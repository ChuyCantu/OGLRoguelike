#pragma once

#include "DungeonGen/Dungeon.hpp"
#include "TurnManager.hpp"

static const std::vector<glm::ivec2> directions8 {
    glm::ivec2{-1,  1},  // North-West
    glm::ivec2{ 0,  1},  // North
    glm::ivec2{ 1,  1},  // North-East
    glm::ivec2{ 1,  0},  // East
    glm::ivec2{ 1, -1},  // South-East
    glm::ivec2{ 0, -1},  // South
    glm::ivec2{-1, -1},  // South-West
    glm::ivec2{-1,  0}   // West
};

bool CanUnitMove(const glm::ivec2& from, const glm::ivec2& to, Unit* unit, Dungeon* dungeon);

bool TryGetAlternativeMove(const glm::ivec2& from, const glm::ivec2& to, const glm::ivec2* followingMove, Unit* unit, 
                           Dungeon* dungeon, glm::ivec2& outAlternativeMove);