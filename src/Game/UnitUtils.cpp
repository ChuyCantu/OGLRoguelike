#include "UnitUtils.hpp"

#include "Algorithms.hpp"

bool CanUnitMove(const glm::ivec2& from, const glm::ivec2& to, Unit* unit, Dungeon* dungeon) {
    DungeonNode& dnode {dungeon->GetNode(to.x, to.y)};
    AStar::Node& anode {dungeon->pathfinding.GetNode(to)};

    if (dnode.unit || anode.isObstacle)
        return false;

    glm::ivec2 dir {to - from};
    // In diagonal movements, check adjacent walls
    if (dir.x != 0 && dir.y != 0) {  // diagonal move
        if (unit->GetDiagonalMovementType() == DiagonalMovement::Never)
            return false;

        // ------------------
        if (unit->GetDiagonalMovementType() != DiagonalMovement::Always) {
            // check if adjacent nodes are obstacles
            AStar::Node& n1 {dungeon->pathfinding.GetNode(glm::ivec2{from.x, to.y})};
            AStar::Node& n2 {dungeon->pathfinding.GetNode(glm::ivec2{to.x, from.y})};

            switch (unit->GetDiagonalMovementType()) {
                case DiagonalMovement::AllowOneObstacle:
                    if (n1.isObstacle && n2.isObstacle)
                        return false;
                    break;
                case DiagonalMovement::OnlyWhenNoObstacles:
                    if (n1.isObstacle || n2.isObstacle)
                        return false;
                    break;
            }
        }
    }

    return true;
}

bool TryGetAlternativeMove(const glm::ivec2& from, const glm::ivec2& to, const glm::ivec2* followingMove, Unit* unit,
                           Dungeon* dungeon, glm::ivec2& outAlternativeMove) {
    // Check adjacent tiles to see if another move can be performed
    auto dir {to - from};
    // find it first in the directions8 list
    int idx = -1;
    for (int i{0}; i < directions8.size(); ++i) {
        if (dir == directions8[i]) {
            idx = i + directions8.size();
            break;
        }
    }

    if (idx < 0) 
        return false;

    std::vector<glm::ivec2> adjacents {directions8[(idx - 1) % directions8.size()], directions8[(idx + 1) % directions8.size()]};

    float dist {std::numeric_limits<float>().infinity()};
    glm::vec2 followingMovef;
    if (followingMove) followingMovef = *followingMove;
    bool foundAlt {false};

    for (size_t i{0}; i < adjacents.size(); ++i) {
        auto& adj {from + adjacents[i]};
        DungeonNode& dnode{dungeon->GetNode(adj.x, adj.y)};
        AStar::Node& anode{dungeon->pathfinding.GetNode(adj)};

        if (!CanUnitMove(from, adj, unit, dungeon)) continue;

        if (followingMove) {
            float tempDist {glm::distance2(followingMovef, glm::vec2{adj})};
            if (tempDist < dist) {
                dist = tempDist;
                outAlternativeMove = adj;
                foundAlt = true;
            }
        }
        else {
            outAlternativeMove = adj;
            return true;
        }
    }

    return foundAlt;
}