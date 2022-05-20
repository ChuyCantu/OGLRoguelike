#pragma once

#include "Game/Algorithms.hpp"
#include "UI/Rect.hpp"
#include "Fov.hpp"

#include <stdint.h>
#include <vector>
#include <entt/core/enum.hpp>

class Unit;

enum NodeType : uint32_t {
    Null = 0,
    Air = 1,
    Ground = 2,
    Wall = 4,

    _entt_enum_as_bitmask
};

struct DungeonNode {
    NodeType type {Air};
    int cost      {1}; //! costs below 1 will cause the algorithm to loop infinitely
    Unit* unit    {nullptr};
};

struct RaycastHitInfo {
    bool hit               {false};
    glm::vec2 intersection {0, 0};
    DungeonNode* node      {nullptr};
    glm::ivec2 nodeCoords  {0, 0};
};

class Dungeon {
public:
    Dungeon() = default;
    
    void CreateNew(const glm::ivec2 size, int minRooms, int maxRooms, 
                   const glm::ivec2& minRoomSize, const glm::ivec2& maxRoomSize);

    DungeonNode& GetNode(int x, int y);
    DungeonNode* TryGetNode(int x, int y);

    //! This is under testing and contain some bugs yet to be used
    RaycastHitInfo Raycast(const glm::vec2& origin, const glm::vec2& dir, float distance, NodeType obstacleFlags = NodeType::Wall);

    const glm::ivec2& GetSize() const {return size; }
    const std::vector<DungeonNode>& GetMap() const { return map; }
    const std::vector<Rect>& GetRooms() const { return rooms; } // room.position is the bottom-left node

private:
    bool OverlapsAnyRoom(const Rect& room, int offset);

//     // !Debug
//     std::vector<double> roomsCenterCoords;
//     std::vector<std::size_t> triangles;
//     std::vector<std::pair<glm::vec2, glm::vec2>> connections;

public:
    AStar astar;
    FovMap fov;

private:
    glm::ivec2 size;
    std::vector<DungeonNode> map;
    std::vector<Rect> rooms;
};