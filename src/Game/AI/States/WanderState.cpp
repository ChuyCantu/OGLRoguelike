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
    auto& transform {owner->GetComponent<Transform>()};
    glm::ivec2 pos {transform.GetPosition().x, transform.GetPosition().y};
    MakeNewRandomPath(pos / TILE_SIZE);

    stillCounter = 0;
}

// #define LONG
void WanderState::Update() {
    if (!TurnManager::Instance().CanPerformNewAction(*owner))
        return;

    if (stillCounter > 2) {
        path.clear();
        stillCounter = 0;
    }

    auto& transform {owner->GetComponent<Transform>()};
    glm::ivec2 pos {transform.GetPosition().x, transform.GetPosition().y};
    glm::ivec2 tiledPos {pos / TILE_SIZE};

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
        int idx = 0;
        for (int i {0}; i < directions.size(); ++i) {
            if (normalizedDest == directions[i]) {
                idx = i + directions.size();
                break;
            }
        }

#ifndef LONG
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
                        case DiagonalMovement::OnlyWhenNoObstacles:
                            if (n1.isObstacle || n2.isObstacle)
                                continue;
                    }
                }

                // foundAlternativePath = true;
                // alt = adj;
                // break;
                // -----------------

                // if (!dungeon->pathfinding.IsDiagonalMovementBlocked(tiledPos, destination, DiagonalMovement::OnlyWhenNoObstacles)) {
                //     foundAlternativePath = true;
                //     alt = adj;
                //     break;
                // }
            } 
            // else {
            //     foundAlternativePath = true;
            //     alt = adj;
            //     break;
            // }

            foundAlternativePath = true;
            alt = adj;
            break;
        }

        if (foundAlternativePath) {
            auto moveAction{MakeOwned<MoveUnitAction>(owner, alt, 0.15f, dungeon)};
            // moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
            if (nextPathNode - 1 >= 0) {
                int nextTileDistance{static_cast<int>(glm::distance2(glm::vec2{alt}, glm::vec2{path[nextPathNode - 1]})) / TILE_SIZE};
                if (nextTileDistance == 1)
                    moveAction->onMoveActionCompleted.Subscribe("OnMoveCompleted", [this]() { --this->nextPathNode; });
            }
            else {
                moveAction->onMoveActionCompleted.Subscribe("OnMoveCompleted", [this]() { --this->nextPathNode; });
            }
            moveAction->onMoveActionCanceled.Subscribe("OnMoveCanceled", [this]() { ++this->stillCounter; });
            owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
            // nextPathNode--;
        }
        else {
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
            ++stillCounter;
        }
#else
        auto& adj1 {tiledPos + directions[(idx - 1) % directions.size()]};
        auto& adj2 {tiledPos + directions[(idx + 1) % directions.size()]};
        DungeonNode& n1 {dungeon->GetNode(adj1.x, adj1.y)};
        DungeonNode& n2 {dungeon->GetNode(adj2.x, adj2.y)};

        // Check id any of the nodes are available
        if (!n1.unit && n1.type == NodeType::Ground) {
            // auto moveAction {MakeOwned<MoveUnitAction>(owner, adj1 * TILE_SIZE, 0.15f, dungeon)};
            // moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
            // owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
            // nextPathNode--;

            // In diagonal movements, check adjacent walls (as it's done in the A* algo)
            if (normalizedDest.x != 0 && normalizedDest.y != 0) { // diagonal move
                // check if adjacent nodes are obstacles
                DungeonNode& n1 {dungeon->GetNode(tiledPos.x, adj1.y)};
                DungeonNode& n2 {dungeon->GetNode(adj1.x, tiledPos.y)};

                if (n1.type != NodeType::Ground && n2.type != NodeType::Ground) {
                   
                }
                else {
                    auto moveAction{MakeOwned<MoveUnitAction>(owner, adj1, 0.15f, dungeon)};
                    moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
                    owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
                    nextPathNode--;
                }
            }
            else {
                auto moveAction{MakeOwned<MoveUnitAction>(owner, adj1, 0.15f, dungeon)};
                moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
                owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
                nextPathNode--;
            }
                
        }
        else if (!n2.unit && n2.type == NodeType::Ground) {
            // auto moveAction {MakeOwned<MoveUnitAction>(owner, adj2 * TILE_SIZE, 0.15f, dungeon)};
            // moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
            // owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
            // nextPathNode--;

            // In diagonal movements, check adjacent walls (as it's done in the A* algo)
            if (normalizedDest.x != 0 && normalizedDest.y != 0) { // diagonal move
                // check if adjacent nodes are obstacles
                DungeonNode& n1 {dungeon->GetNode(tiledPos.x, adj2.y)};
                DungeonNode& n2 {dungeon->GetNode(adj2.x, tiledPos.y)};

                if (n1.type != NodeType::Ground && n2.type != NodeType::Ground) {
                    owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
                }
                else {
                    auto moveAction{MakeOwned<MoveUnitAction>(owner, adj2, 0.15f, dungeon)};
                    moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
                    owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
                    nextPathNode--;
                }
            }
            else {
                auto moveAction{MakeOwned<MoveUnitAction>(owner, adj2, 0.15f, dungeon)};
                moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
                owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
                nextPathNode--;
            }
        }
        else
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
    }
    else {
        auto moveAction {MakeOwned<MoveUnitAction>(owner, path[nextPathNode--], 0.15f, dungeon)};
        moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
        owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
#endif
    }
    else {
        // auto moveAction {MakeOwned<MoveUnitAction>(owner, path[nextPathNode--], 0.15f, dungeon)};
        // moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
        // moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", [this]() { this->nextPathNode = std::min(this->nextPathNode + 1, static_cast<int>(this->path.size() - 2)); });
        auto moveAction {MakeOwned<MoveUnitAction>(owner, path[nextPathNode], 0.15f, dungeon)};
        moveAction->onMoveActionCompleted.Subscribe("OnMoveCompleted", [this]() { --this->nextPathNode; });
        moveAction->onMoveActionCanceled.Subscribe("OnMoveCanceled", [this]() { ++this->stillCounter; });
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
        nextPathNode = -1;
        owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
        ++stillCounter;
    }

    nextPathNode = path.size() - 2; // -2 since the path include the start position
}

void WanderState::OnMoveActionCanceled() {
    nextPathNode = std::min(nextPathNode + 1, static_cast<int>(path.size() - 2));
}