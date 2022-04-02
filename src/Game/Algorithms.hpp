#pragma once

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

enum class DiagonalMovement {
    Always, AllowOneObstacle, OnlyWhenNoObstacles, Never
};

class AStar {
public:
    struct Node {
        bool isObstacle {false};
        int cost        {1}; //! costs below 1 will cause the algorithm to loop infinitely

        bool visited {false};
        int f        {0}; // total cost
        int g        {0}; // cost of cheapest path from start node
        int h        {0}; // heuristics
    };

    AStar();
    AStar(int mapWidth, int mapHeight);

    void CreateMap(int mapWidth, int mapHeight);
    void Clear();

    /**
     * @brief Finds the shortest available path from start to goal
     * 
     * @param start start node position
     * @param goal end node position
     * @param outPath a reversed vector of the nodes that form the path (path[0] will be the goal node)
     * @param diagonalMovement defines how the algorithm will react to diagonal movements
     * @return true if a valid path to the goal was found
     */
    bool FindPath(const glm::ivec2& start, const glm::ivec2& goal, std::vector<glm::ivec2>& outPath, DiagonalMovement diagonalMovement = DiagonalMovement::Never);
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