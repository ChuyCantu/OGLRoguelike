#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <vector>

//+ Dungeon Generation Algorthims: ======================================================

// https://en.wikipedia.org/wiki/Disjoint-set_data_structure
struct DisjointSet {
    std::unordered_map<glm::vec2, glm::vec2> parents;

    void MakeSet(const std::vector<glm::vec2>& data);
    const glm::vec2& FindSet(const glm::vec2& v);
    void Union(const glm::vec2& v, const glm::vec2& w);
};

struct Edge {
    Edge(glm::vec2& a, glm::vec2& b);

    glm::vec2* a;
    glm::vec2* b;

    bool operator==(const Edge& other);
};

// https://en.wikipedia.org/wiki/Kruskal%27s_algorithm
// extra: https://www.techiedelight.com/kruskals-algorithm-for-finding-minimum-spanning-tree/
void KruskalMinumumSpaningTree(const std::vector<glm::vec2>& vertices, const std::vector<Edge>& edges, std::vector<Edge>& outMST, float keepDeletedPathProbability = 0.0f);

//+ A* Pathfinding: ======================================================

class AStar {
public:
    struct Node {
        bool isObstacle {false};
        int cost        {1};

        bool visited {false};
        int f        {0}; // total cost
        int g        {0}; // cost of cheapest path from start node
        int h        {0}; // heuristics
    };

    AStar();
    AStar(int mapWidth, int mapHeight);

    void CreateMap(int mapWidth, int mapHeight);
    void Clear();
    bool FindPath(const glm::ivec2& start, const glm::ivec2& goal, std::vector<glm::ivec2>& outPath, bool allowDiagonalMovement = false);
    void ResetNodes();
    Node& GetNode(const glm::ivec2& position);
    Node* TryGetNode(const glm::ivec2& position);

    const glm::ivec2& GetSize() const { return size; }

    std::vector<Node>::iterator begin() { return map.begin(); }
    std::vector<Node>::iterator end() { return map.end(); }

private:
    void ReconstructPath(std::unordered_map<glm::ivec2, glm::ivec2>& parents, 
                         const glm::ivec2& start, const glm::ivec2& goal, 
                         std::vector<glm::ivec2>& outPath);

private:
    glm::ivec2 size;
    std::vector<Node> map;
};

#endif // __ALGORITHMS_H__