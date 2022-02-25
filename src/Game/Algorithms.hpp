#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <vector>

//+ Dungeon Generation Algorthims:

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

// TODO: A* Pathfinding

#endif // __ALGORITHMS_H__