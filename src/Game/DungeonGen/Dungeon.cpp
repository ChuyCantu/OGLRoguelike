#include "Dungeon.hpp"

#include "Core/Log.hpp"
#include "Game/Algorithms.hpp"
#include "Utils/Random.hpp"

#include <delaunator.hpp>
#include <glm/gtx/norm.hpp>

void Dungeon::CreateNew(const glm::ivec2 size, int minRooms, int maxRooms,
               const glm::ivec2& minRoomSize, const glm::ivec2& maxRoomSize) {
    //+ Create rooms:
    this->size = size;
    if (!rooms.empty()) rooms.clear();
    if (!roomsCenterCoords.empty())  roomsCenterCoords.clear();
    std::vector<glm::ivec2> roomsCenter;
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
        roomsCenter.push_back(glm::ivec2{room.position + room.size / 2.f});

        roomsCenterCoords.push_back(static_cast<int>(room.position.x + room.size.x / 2));
        roomsCenterCoords.push_back(static_cast<int>(room.position.y + room.size.y / 2));
    }

    int wallThickness {1};
    ASSERT(wallThickness > padding, "Wall thickness may cause an argument out of bounds since it's bigger than the padding given to the rooms.");
    for (Rect& room : rooms) {
        for (int y{-wallThickness}; y < room.size.y + wallThickness; ++y) {
            for (int x{-wallThickness}; x < room.size.x + wallThickness; ++x) {
                DungeonNode& node{GetNode(room.position.x + x, room.position.y + y)};
                if (x < 0 || y < 0 || x >= room.size.x || y >= room.size.y) {
                    if (node.type != NodeType::Ground) {
                        node.cost = 5;
                        node.type = NodeType::Wall;
                    }
                }
                else {
                    node.type = NodeType::Ground;
                    node.cost = 5;
                }
            }
        }
    }

    //+ Find minimum span tree to be used for the paths:
    delaunator::Delaunator graph {roomsCenterCoords};
    triangles = graph.triangles;  // Debug

    std::vector<glm::vec2> vertices(triangles.size()); // pre-allocate so we can use pointers to the vec2 safely
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
    if (!connections.empty()) connections.clear();
    for (auto& edge : mst) 
        connections.emplace_back(std::make_pair(*edge.a, *edge.b));
}

DungeonNode& Dungeon::GetNode(int x, int y) {
    ASSERT(x < 0 || y < 0 || x >= size.x || y >= size.y, "Arguments of out bounds.");
    return map[x + y * size.x];
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