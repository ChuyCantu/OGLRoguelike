#include "Algorithms.hpp"

#include "Utils/Random.hpp"

#include <glm/gtx/norm.hpp>

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