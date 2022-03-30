#include "WanderState.hpp"

#include "Game/Action.hpp"
#include "Game/Constants.hpp"
#include "Game/DungeonGen/Dungeon.hpp"
#include "Game/TurnManager.hpp"
#include "Utils/Random.hpp"

WanderState::WanderState(Unit* owner, Dungeon* dungeon) 
    : State{"WanderState"}, owner{owner}, dungeon{dungeon} { }

void WanderState::OnEnter() {
    // path.clear();
    // nextPathNode = -1;
    // LOG_TRACE("Entered wander");
}

void WanderState::Update() {
    if (!TurnManager::Instance().CanPerformNewAction(*owner))
        return;

    auto& transform {owner->GetComponent<Transform>()};
    glm::ivec2 pos {transform.GetPosition().x, transform.GetPosition().y};
    glm::ivec2 tiledPos {pos / TILE_SIZE};

    if (path.empty() || nextPathNode < 0) {
        const Rect& room {dungeon->GetRooms()[Random::Range(0, dungeon->GetRooms().size() - 1)]};
        glm::ivec2 pathGoal {Random::Range(static_cast<int>(room.position.x), 
                                           static_cast<int>(room.position.x + room.size.x) - 1),
                             Random::Range(static_cast<int>(room.position.y), 
                                           static_cast<int>(room.position.y + room.size.y) - 1)};

        if (!dungeon->pathfinding.FindPath(tiledPos, pathGoal, path, DiagonalMovement::OnlyWhenNoObstacles)) {
            path.clear();
            nextPathNode = -1;
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
        }

        nextPathNode = path.size() - 2;
    }

    if (nextPathNode < 0)
        return;

    auto& destination {path[nextPathNode]};
    if (dungeon->GetNode(destination.x, destination.y).unit) {
        // Check adjacent tiles to see if another move can be performed
        static const std::vector<glm::ivec2> adjacents {
            glm::ivec2{-1,  1},  // North-West
            glm::ivec2{ 0,  1},  // North
            glm::ivec2{ 1,  1},  // North-East
            glm::ivec2{ 1,  0},  // East
            glm::ivec2{ 1, -1},  // South-East
            glm::ivec2{ 0, -1},  // South
            glm::ivec2{-1, -1},  // South-West
            glm::ivec2{-1,  0}   // West
        };
        auto normalizedDest {destination - tiledPos};
        // find it first in the adjacents list
        int idx = 0;
        for (int i {0}; i < adjacents.size(); ++i) {
            if (normalizedDest == adjacents[i]) {
                idx = i + adjacents.size();
                break;
            }
        }
        auto& adj1 {tiledPos + adjacents[(idx - 1) % adjacents.size()]};
        auto& adj2 {tiledPos + adjacents[(idx + 1) % adjacents.size()]};
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
                    owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
                }
                else {
                    auto moveAction{MakeOwned<MoveUnitAction>(owner, adj1 * TILE_SIZE, 0.15f, dungeon)};
                    moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
                    owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
                    nextPathNode--;
                }
            }
            else {
                auto moveAction{MakeOwned<MoveUnitAction>(owner, adj1 * TILE_SIZE, 0.15f, dungeon)};
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
                    auto moveAction{MakeOwned<MoveUnitAction>(owner, adj2 * TILE_SIZE, 0.15f, dungeon)};
                    moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
                    owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
                    nextPathNode--;
                }
            }
            else {
                auto moveAction{MakeOwned<MoveUnitAction>(owner, adj2 * TILE_SIZE, 0.15f, dungeon)};
                moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
                owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
                nextPathNode--;
            }
        }
        else
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
    }
    else {
        auto moveAction {MakeOwned<MoveUnitAction>(owner, path[nextPathNode--] * TILE_SIZE, 0.15f, dungeon)};
        moveAction->onMoveActionCanceled.Subscribe("OnMoveActionCanceled", &WanderState::OnMoveActionCanceled, this);
        owner->GetComponent<UnitComponent>().SetAction(std::move(moveAction));
    }
}

void WanderState::OnExit() {

}

void WanderState::OnMoveActionCanceled() {
    ++nextPathNode;
}