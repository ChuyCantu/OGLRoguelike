#include "Algorithms.hpp"

#include "Core/Log.hpp"
#include "Utils/Random.hpp"

#include <glm/gtx/norm.hpp>
#include <queue>

//+ Dungeon Generation Algorthims: ======================================================

void DisjointSet::MakeSet(const std::vector<glm::vec2>& data) {
    for (auto& v : data) {
        parents[v] = v;
    }
}

const glm::vec2& DisjointSet::FindSet(const glm::vec2& v) {
    if (parents[v] != v) {
        parents[v] = FindSet(parents[v]);
        return parents[v];
    }
    return v;
}

void DisjointSet::Union(const glm::vec2& v, const glm::vec2& w) {
    auto& vF {FindSet(v)};
    auto& wF {FindSet(w)};
    auto pv {v - vF};
    auto pw {w - wF};
    float dv {glm::length2(pv)};
    float dw {glm::length2(pw)};
    if (dv < dw) {
        parents[vF] = wF;
    }
    else if (dv > dw) {
        parents[wF] = vF;
    }
    else {
        parents[wF] = vF;
    }
}

Edge::Edge(glm::vec2& a, glm::vec2& b) : a{&a}, b{&b} { }

bool Edge::operator==(const Edge& other) {
    return *a == *other.a && *b == *other.b || 
           *a == *other.b && *b == *other.a;
}

void KruskalMinumumSpaningTree(const std::vector<glm::vec2>& vertices, const std::vector<Edge>& edges, std::vector<Edge>& outMST, float keepDeletedPathProbability) {
    DisjointSet set;

    set.MakeSet(vertices);

    for (const Edge& edge : edges) {
        auto& v {set.FindSet(*edge.a)};
        auto& w {set.FindSet(*edge.b)};

        if (v != w) {
            outMST.emplace_back(edge);
            set.Union(v, w);
        }
        else if (keepDeletedPathProbability > 0) {
            if (Random::Next() <= keepDeletedPathProbability)  // Randomly add a deleted edge for a few extra paths
                outMST.emplace_back(edge);
        }
    }
}

//+ A* Pathfinding: ======================================================

AStar::AStar() { }

AStar::AStar(int mapWidth, int mapHeight) 
    : size{mapWidth, mapHeight}, map(mapWidth * mapHeight) { }

void AStar::CreateMap(int mapWidth, int mapHeight) {
    size = glm::ivec2{mapWidth, mapHeight};
    map = std::vector<AStar::Node>(mapWidth * mapHeight);
}

void AStar::Clear() {
    size = glm::ivec2{0, 0};
    map.clear();
}

AStar::Node& AStar::GetNode(const glm::ivec2& position) {
    ASSERT(position.x < 0 || position.y < 0 || position.x >= size.x || position.y >= size.y, "Index out of bounds");
    return map[position.x + position.y * size.x];
}

AStar::Node* AStar::TryGetNode(const glm::ivec2& position) {
    if (position.x < 0 || position.y < 0 || position.x >= size.x || position.y >= size.y)
        return nullptr;
    return &map[position.x + position.y * size.x];
}

static const std::vector<glm::ivec2> neighbors4{
    glm::ivec2{ 0,  1},  // North
    glm::ivec2{ 1,  0},  // East
    glm::ivec2{ 0, -1},  // South
    glm::ivec2{-1,  0},  // West
};

static const std::vector<glm::ivec2> neighbors8 {
    glm::ivec2{ 0,  1}, // North
    glm::ivec2{ 1,  1}, // North-East
    glm::ivec2{ 1,  0}, // East
    glm::ivec2{ 1, -1}, // South-East
    glm::ivec2{ 0, -1}, // South
    glm::ivec2{-1, -1}, // South-West
    glm::ivec2{-1,  0}, // West
    glm::ivec2{-1,  1}  // North-West
};

int ManhattanDistance(const glm::ivec2& p1, const glm::ivec2& p2) {
    return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
}

int DiagonalDistance(const glm::ivec2& p1, const glm::ivec2& p2, int D, int D2) {
    int dx = abs(p1.x - p2.x);
    int dy = abs(p1.y - p2.y);
    return D * (dx + dy) + (D2 - 2 * D) * std::min(dx, dy);
}

// Special fix to prefer no diagonal movement
int GetCost(const glm::ivec2& current, const glm::ivec2& next, AStar& astar) {
    glm::ivec2 p{next - current};
    // If costs change to float, only sum something small like 0.001f
    if ((p.x + p.y) % 2 == 0)  // Diagonal
        return astar.GetNode(next).cost + 1;
    // if ((p.x + p.y) % 2 == 1)
    //     return astar.GetNode(next).cost;

    return astar.GetNode(next).cost;  //* No extra cost for diagonals
}

struct NodePair {
    glm::ivec2 position;
    AStar::Node* node;

    friend bool operator>(const NodePair& a, const NodePair& b);
};

bool operator>(const NodePair& a, const NodePair& b) {
    return a.node->f > b.node->f;
}

// TODO: Prevent infinite loop when no path is found

// https://en.wikipedia.org/wiki/A*_search_algorithm
// http://theory.stanford.edu/~amitp/GameProgramming/
bool AStar::FindPath(const glm::ivec2& start, const glm::ivec2& goal, std::vector<glm::ivec2>& outPath, bool allowDiagonalMovement) {
    Node* startNode {TryGetNode(start)};
    
    if (start == goal || !startNode || !TryGetNode(goal)) 
        return false;

    ResetNodes();

    const std::vector<glm::ivec2>& neighbors { allowDiagonalMovement ? neighbors8 : neighbors4 };

    std::priority_queue<NodePair, std::vector<NodePair>, std::greater<NodePair>> open;
    open.emplace(NodePair{start, startNode});
    startNode->g = 0;
    // startNode->f = 0;

    std::unordered_map<glm::ivec2, glm::ivec2> cameFrom;
    // cameFrom[start] = start;

    while (!open.empty()) {
        NodePair current {open.top()}; // Node with lowest f score value

        if (current.position == goal) 
            break;

        open.pop();

        current.node->visited = true;

        for (auto& neighbor : neighbors) {
            glm::ivec2 next {current.position + neighbor};

            Node* nextNode {TryGetNode(next)};
            if (!nextNode || nextNode->isObstacle || nextNode->visited)
                continue;
            
            int tentativeNewCost {nextNode->g + GetCost(current.position, next, *this)};
            nextNode->visited = true;

            if (tentativeNewCost < nextNode->g) {
                nextNode->g = tentativeNewCost;
                nextNode->h = allowDiagonalMovement ? DiagonalDistance(next, goal, 1, 1) : ManhattanDistance(next, goal);
                nextNode->f = tentativeNewCost + nextNode->h;
                cameFrom[next] = current.position;

                open.emplace(NodePair{next, nextNode});
            }
        }
    }

    if (cameFrom.empty())
        return false;

    ReconstructPath(cameFrom, start, goal, outPath);
    return true;
}

void AStar::ResetNodes() {
    int infinity {INT_MAX};
    for (Node& node : map) {
        node.visited = false;
        node.f = infinity;
        node.g = infinity;
        node.h = 0;
    }
}

void AStar::ReconstructPath(std::unordered_map<glm::ivec2, glm::ivec2>& parents, 
                            const glm::ivec2& start, const glm::ivec2& goal, 
                            std::vector<glm::ivec2>& outPath) {
    if (!outPath.empty()) outPath.clear();

    const glm::ivec2* current {&goal};
    while (*current != start) {
        outPath.push_back(*current);
        current = &parents[*current];
    }
    outPath.push_back(start);
}