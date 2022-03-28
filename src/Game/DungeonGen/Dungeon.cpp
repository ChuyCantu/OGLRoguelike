#include "Dungeon.hpp"

#include "Core/Log.hpp"
#include "Utils/Random.hpp"

#include <delaunator.hpp>
#include <glm/gtx/norm.hpp>

void Dungeon::CreateNew(const glm::ivec2 size, int minRooms, int maxRooms,
               const glm::ivec2& minRoomSize, const glm::ivec2& maxRoomSize) {
    //+ Create rooms:
    this->size = size;
    if (!rooms.empty()) rooms.clear();
    // if (!roomsCenterCoords.empty())  roomsCenterCoords.clear();
    std::vector<double> roomsCenterCoords;
    map = std::vector<DungeonNode>(size.x * size.y);
    
    int roomsToMake {Random::Range(minRooms, maxRooms)};
    int padding {2};
    for (int i{0}; i < roomsToMake; ++i) {
        Rect room;
        int creations {0};
        do {
            room.size = {Random::Range(minRoomSize.x, maxRoomSize.x), Random::Range(minRoomSize.x, maxRoomSize.y)};
            room.position = glm::vec2{Random::Range(padding, static_cast<int>(size.x - room.size.x) - padding), 
                                      Random::Range(padding, static_cast<int>(size.y - room.size.y) - padding)};
            ++creations;                                    
        } while (OverlapsAnyRoom(room, 3) && creations < 10);
        LOGIF_DEBUG(creations >= 10, "Pos: {}, {}", room.position.x, room.position.y);
        rooms.push_back(room);

        roomsCenterCoords.push_back(static_cast<int>(room.position.x + room.size.x / 2));
        roomsCenterCoords.push_back(static_cast<int>(room.position.y + room.size.y / 2));
    }

    //+ Add rooms to map
    int wallThickness {1};
    ASSERT(wallThickness > padding, "Wall thickness may cause an argument out of bounds since it's bigger than the padding given to the rooms.");
    for (Rect& room : rooms) {
        for (int y{-wallThickness}; y < room.size.y + wallThickness; ++y) {
            for (int x{-wallThickness}; x < room.size.x + wallThickness; ++x) {
                DungeonNode& node{GetNode(room.position.x + x, room.position.y + y)};
                if (x < 0 || y < 0 || x >= room.size.x || y >= room.size.y) {
                    if (node.type != NodeType::Ground) {
                        node.type = NodeType::Wall;
                        node.cost = 10;
                    }
                }
                else {
                    node.type = NodeType::Ground;
                    node.cost = 1;
                }
            }
        }
    }

    //+ Create some extra paths:
    int extraPaths {Random::Range(1, 10)}; 
    for (int i{0}; i < roomsToMake; ++i) {
        glm::ivec2 pos;
        pos.x = Random::Range(padding, size.x - 1 - padding); // This nodes are always 1x1 
        pos.y = Random::Range(padding, size.y - 1 - padding);                                   

        // added as room so it can be used to connect paths
        roomsCenterCoords.push_back(pos.x);
        roomsCenterCoords.push_back(pos.y);

        //+ Add extra path to map
        DungeonNode& node{GetNode(pos.x, pos.y)};
        node.type = NodeType::NodeTypeCount; // Ground
    }

    //+ Find minimum span tree between rooms (and extra path nodes):
    delaunator::Delaunator graph {roomsCenterCoords};
    // triangles = graph.triangles;  // Debug

    std::vector<glm::vec2> vertices(graph.triangles.size()); // pre-allocate so we can use pointers to the vec2 safely
    std::vector<Edge> edges;

    for (size_t i = 0; i < graph.triangles.size(); i += 3) {
        vertices[i].x     = roomsCenterCoords[2 * graph.triangles[i]];
        vertices[i].y     = roomsCenterCoords[2 * graph.triangles[i] + 1];
        vertices[i + 1].x = roomsCenterCoords[2 * graph.triangles[i + 1]];
        vertices[i + 1].y = roomsCenterCoords[2 * graph.triangles[i + 1] + 1];
        vertices[i + 2].x = roomsCenterCoords[2 * graph.triangles[i + 2]],
        vertices[i + 2].y = roomsCenterCoords[2 * graph.triangles[i + 2] + 1];

        edges.emplace_back(vertices[i]    , vertices[i + 1]);
        edges.emplace_back(vertices[i + 1], vertices[i + 2]);
        edges.emplace_back(vertices[i + 2], vertices[i]);
    }

    // Sort from lowest to highest weight (considered the distance between vertices in this case)
    std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
        return glm::distance2(*e1.a, *e1.b) < glm::distance2(*e2.a, *e2.b);
    });
    // Erase repeated edges
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());

    std::vector<Edge> mst;
    KruskalMinumumSpaningTree(vertices, edges, mst, 0.1f);
    //! Debug:
    // if (!connections.empty()) connections.clear();

    // for (auto& edge : mst) 
    //     connections.emplace_back(std::make_pair(*edge.a, *edge.b));

    // for (auto& edge : edges)
    //     connections.emplace_back(std::make_pair(*edge.a, *edge.b));

    // TODO: Instead of joining rooms by their center, randomly create doors to join
    //+ Create paths with the minimum span tree:
    for (auto& edge : mst) {
        glm::ivec2 from {*edge.a};
        glm::ivec2 to   {*edge.b};

        bool fromMovesHoriz {Random::Next() >= 0.5f};

        glm::ivec2 diff {to - from};
        if (fromMovesHoriz) {
            // connections.emplace_back(std::make_pair(from, from + glm::ivec2{diff.x, 0.f}));
            // connections.emplace_back(std::make_pair(to, to - glm::ivec2{0.0f, diff.y}));

            glm::ivec2 end {from + glm::ivec2{diff.x, 0.f}};
            for (int x {std::min(from.x, end.x)}; x <= std::max(from.x, end.x); ++x) { 
                DungeonNode& currentNode {GetNode(x, from.y)};
                currentNode.type = NodeType::Ground;
                currentNode.cost = 1;

                for (int y {from.y - 1}; y <= (from.y + 1); ++y) {
                    for (int i {x - 1}; i <= (x + 1); ++i) {
                        auto& node {GetNode(i, y)};
                        if (node.type != NodeType::Ground)
                            node.type = NodeType::Wall;
                    }
                }
            }

            end = to - glm::ivec2{0.0f, diff.y};
            for (int y {std::min(to.y, end.y)}; y <= std::max(to.y, end.y); ++y) {
                DungeonNode& currentNode {GetNode(to.x, y)};
                currentNode.type = NodeType::Ground;
                currentNode.cost = 1;

                for (int x {to.x - 1}; x <= (to.x + 1); ++x) {
                    for (int i {y - 1}; i <= (y + 1); ++i) {
                        auto& node {GetNode(x, i)};
                        if (node.type != NodeType::Ground)
                            node.type = NodeType::Wall;
                    }
                }
            }
        }
        else {
            // connections.emplace_back(std::make_pair(to, to - glm::ivec2{diff.x, 0.0f}));
            // connections.emplace_back(std::make_pair(from, from + glm::ivec2{0.f, diff.y}));

            glm::ivec2 end {from + glm::ivec2{0.f, diff.y}};
            for (int y {std::min(from.y, end.y)}; y <= std::max(from.y, end.y); ++y) {
                DungeonNode& currentNode {GetNode(from.x, y)};
                currentNode.type = NodeType::Ground;
                currentNode.cost = 1;

                for (int x {from.x - 1}; x <= (from.x + 1); ++x) {
                    for (int i {y - 1}; i <= (y + 1); ++i) {
                        auto& node {GetNode(x, i)};
                        if (node.type != NodeType::Ground)
                            node.type = NodeType::Wall;
                    }
                }
            }

            end = to - glm::ivec2{diff.x, 0.0f};
            for (int x {std::min(to.x, end.x)}; x <= std::max(to.x, end.x); ++x) {
                DungeonNode& currentNode {GetNode(x, to.y)};
                currentNode.type = NodeType::Ground;
                currentNode.cost = 1;

                for (int y {to.y - 1}; y <= (to.y + 1); ++y) {
                    for (int i {x - 1}; i <= (x + 1); ++i) {
                        auto& node {GetNode(i, y)};
                        if (node.type != NodeType::Ground)
                            node.type = NodeType::Wall;
                    }
                }
            }
        }
    }

    // Create A* pathfinding
    pathfinding.CreateMap(size.x, size.y);
    for (int y{0}; y < size.y; ++y) {
        for (int x{0}; x < size.x; ++x) {
            DungeonNode& node {GetNode(x, y)};
            if (node.type == NodeType::Air || node.type == NodeType::Wall)
                pathfinding.GetNode({x, y}).isObstacle = true;
            // if (node.type == NodeType::Wall)
            //     pathfinding.GetNode({x, y}).isObstacle = true;
            // else if (node.type == NodeType::Ground)
                // pathfinding.GetNode({x, y}).cost = node.cost;

            pathfinding.GetNode({x, y}).cost = node.cost;
        }
    }
}

DungeonNode& Dungeon::GetNode(int x, int y) {
    ASSERT(x < 0 || y < 0 || x >= size.x || y >= size.y || map.empty(), "Arguments of out bounds.");
    return map[x + y * size.x];
}

DungeonNode* Dungeon::TryGetNode(int x, int y) {
    if (x < 0 || y < 0 || x >= size.x || y >= size.y || map.empty())
        return nullptr;
    return &map[x + y * size.x];
}

bool Dungeon::OverlapsAnyRoom(const Rect& room, int offset) {
    Rect src {{room.position.x - offset, room.position.y - offset},
              {room.size.x + offset * 2, room.size.y + offset * 2}};
    for (auto& other : rooms) {
        if (other.Intersect(src))
            return true;
    }
    return false;
}