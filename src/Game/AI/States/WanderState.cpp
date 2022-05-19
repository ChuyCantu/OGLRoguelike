#include "WanderState.hpp"

#include "Game/Action.hpp"
#include "Game/Constants.hpp"
#include "Game/DungeonGen/Dungeon.hpp"
#include "Game/TurnManager.hpp"
#include "Utils/Random.hpp"

static const std::vector<glm::ivec2> directions {
    glm::ivec2{-1,  1},  // North-West
    glm::ivec2{ 0,  1},  // North
    glm::ivec2{ 1,  1},  // North-East
    glm::ivec2{ 1,  0},  // East
    glm::ivec2{ 1, -1},  // South-East
    glm::ivec2{ 0, -1},  // South
    glm::ivec2{-1, -1},  // South-West
    glm::ivec2{-1,  0}   // West
};

WanderState::WanderState(Unit* owner, Dungeon* dungeon) 
    : State{"WanderState"}, owner{owner}, dungeon{dungeon} { }

void WanderState::OnEnter() {
    // path.clear();
    // nextPathNode = -1;
    // LOG_TRACE("Entered wander");
    
    // auto& transform {owner->GetComponent<Transform>()};
    // glm::ivec2 pos {transform.GetPosition().x, transform.GetPosition().y};
    // MakeNewRandomPath(pos / TILE_SIZE);

    path.clear();
    nextPathNode = -1;

    stillCounter = 0;
}

// TODO: Fix enemy sometimes recalculating path immediately after avoiding an obstacle or sometimes skipping one block if the player just positioned in the node to be visited
void WanderState::Update() {
    // if actions are set up as soon as the player finishes its turn, all async actions can start at the same time, preventing delays between frames
    auto currUnit {TurnManager::Instance().GetCurrentUnit()};
    if (owner->GetComponent<UnitComponent>().GetAction() || (currUnit && currUnit->tag == "Player"))
        return;

    if (stillCounter > 2) {
        path.clear();
        nextPathNode = -1;
        stillCounter = 0;
    }

    // auto& transform {owner->GetComponent<Transform>()};
    // glm::ivec2 pos {transform.GetPosition().x, transform.GetPosition().y};
    // glm::ivec2 tiledPos {pos / TILE_SIZE};
    auto& tiledPos {owner->GetComponent<UnitComponent>().GetPosition()};

    if (path.empty() || nextPathNode < 0) {
        do {
            MakeNewRandomPath(tiledPos);
        } while (nextPathNode < 0);
    }

    if (nextPathNode < 0)
        return;

    auto& destination {path[nextPathNode]};
    if (dungeon->GetNode(destination.x, destination.y).unit) {
        // Check adjacent tiles to see if another move can be performed
        auto normalizedDest {destination - tiledPos};
        // find it first in the directions list
        int idx = -1;
        for (int i {0}; i < directions.size(); ++i) {
            if (normalizedDest == directions[i]) {
                idx = i + directions.size();
                break;
            }
        }

        if (idx < 0) {
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
            path.clear();
            nextPathNode = -1;
            stillCounter = 0;
            return;
        }

        std::vector<glm::ivec2> adjacents { directions[(idx - 1) % directions.size()], directions[(idx + 1) % directions.size()] };

        bool foundAlternativePath {false};
        glm::ivec2 alt;
        for (size_t i {0}; i < adjacents.size(); ++i) {
            auto& adj {tiledPos + adjacents[i]};
            DungeonNode& dnode {dungeon->GetNode(adj.x, adj.y)};
            AStar::Node& anode {dungeon->pathfinding.GetNode(adj)};

            if (dnode.unit || anode.isObstacle) 
                continue;

            // In diagonal movements, check adjacent walls 
            auto newNormalizedDest {adj - tiledPos};
            if (newNormalizedDest.x != 0 && newNormalizedDest.y != 0) {  // diagonal move
                if (owner->GetDiagonalMovementType() == DiagonalMovement::Never)
                    continue;

                // ------------------
                if (owner->GetDiagonalMovementType() != DiagonalMovement::Always) {
                    // check if adjacent nodes are obstacles
                    AStar::Node& n1 {dungeon->pathfinding.GetNode(glm::ivec2{tiledPos.x, adj.y})};
                    AStar::Node& n2 {dungeon->pathfinding.GetNode(glm::ivec2{adj.x, tiledPos.y})};

                    switch (owner->GetDiagonalMovementType()) {
                        case DiagonalMovement::AllowOneObstacle:
                            if (n1.isObstacle && n2.isObstacle)
                                continue;
                            break;
                        case DiagonalMovement::OnlyWhenNoObstacles:
                            if (n1.isObstacle || n2.isObstacle)
                                continue;
                            break;
                    }
                }
            } 

            foundAlternativePath = true;
            alt = adj;
            break;
        }

        if (foundAlternativePath) {
            auto moveAction{MakeOwned<MoveUnitAction>(owner, alt, 0.15f, dungeon)};
            if (nextPathNode - 1 >= 0) {
                int nextTileDistance{static_cast<int>(glm::distance2(glm::vec2{alt}, glm::vec2{path[nextPathNode - 1]}))};
                if (nextTileDistance <= 2)
                    moveAction->onMoveActionCompleted.Subscribe("OnMoveCompleted", [this]() { --this->nextPathNode; });
            }
            else {
                moveAction->onMoveActionCompleted.Subscribe("OnMoveCompleted", [this]() { --this->nextPathNode; });
            }
            moveAction->onMoveActionCanceled.Subscribe("OnMoveCanceled", [this]() {nextPathNode = std::min(nextPathNode + 1, static_cast<int>(path.size() - 1)); /*++this->stillCounter;*/ });
            owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
        }
        else {
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
            ++stillCounter;
        }
    }
    else {
        auto moveAction {MakeOwned<MoveUnitAction>(owner, path[nextPathNode], 0.15f, dungeon)};
        moveAction->onMoveActionCompleted.Subscribe("OnMoveCompleted", [this]() { --this->nextPathNode; });
        moveAction->onMoveActionCanceled.Subscribe("OnMoveCanceled", [this]() { nextPathNode = std::min(nextPathNode + 1, static_cast<int>(path.size() - 1)); /*++this->stillCounter;*/ });
        owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
    }
}

void WanderState::OnExit() {

}

void WanderState::MakeNewRandomPath(const glm::ivec2& startPos) {
    const Rect& room{dungeon->GetRooms()[Random::Range(0, dungeon->GetRooms().size() - 1)]};
    glm::ivec2 pathGoal{Random::Range(static_cast<int>(room.position.x),
                                      static_cast<int>(room.position.x + room.size.x) - 1),
                        Random::Range(static_cast<int>(room.position.y),
                                      static_cast<int>(room.position.y + room.size.y) - 1)};

    if (!dungeon->pathfinding.FindPath(startPos, pathGoal, path, owner->GetDiagonalMovementType())) {
        path.clear();
        owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
        ++stillCounter;
    }

    nextPathNode = path.size() - 1;
}