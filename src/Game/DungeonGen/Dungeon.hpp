#pragma once

#include "Game/Algorithms.hpp"
#include "UI/Rect.hpp"

#include <stdint.h>
#include <vector>

struct DungeonNode {
    uint32_t type {0};
    int cost      {1}; //! costs below 1 will cause the algorithm to loop infinitely
    class Unit* unit {nullptr};
};

enum NodeType : uint32_t {
    Air = 0, Ground, Wall,

    NodeTypeCount
};

class Dungeon {
public:
    Dungeon() = default;
    
    void CreateNew(const glm::ivec2 size, int minRooms, int maxRooms, 
                   const glm::ivec2& minRoomSize, const glm::ivec2& maxRoomSize);

    DungeonNode& GetNode(int x, int y);
    DungeonNode* TryGetNode(int x, int y);

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
    AStar pathfinding;

private:
    glm::ivec2 size;
    std::vector<DungeonNode> map;
    std::vector<Rect> rooms;
};