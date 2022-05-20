#include "ChaseState.hpp"

#include "Game/Action.hpp"
#include "Game/DungeonGen/Dungeon.hpp"
#include "Game/TurnManager.hpp"
#include "Game/UnitUtils.hpp"

ChaseState::ChaseState(Unit* owner, entt::entity targetRef, Dungeon* dungeon)
    : State{"ChaseState"}, owner{owner}, targetRef{targetRef}, dungeon{dungeon} {
}

void ChaseState::OnEnter() {
    path.clear();
    nextPathNode = -1;

    stillCounter = 0;

    auto target {owner->FindGameObject(targetRef)};
    if (target)
        prevTargetPos = target->GetComponent<UnitComponent>().GetPosition();
}

void ChaseState::Update() {
    // if actions are set up as soon as the player finishes its turn, all async actions can start at the same time, preventing delays between frames
    auto currUnit {TurnManager::Instance().GetCurrentUnit()};
    if (owner->GetComponent<UnitComponent>().GetAction() || (currUnit && currUnit->tag == "Player"))
        return;

    auto& ownerPos {owner->GetComponent<UnitComponent>().GetPosition()};
    auto target {owner->FindGameObject(targetRef)};
    if (!target) {
        owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
        return;
    }

    auto& targetPos {target->GetComponent<UnitComponent>().GetPosition()};

    if (stillCounter > 1 || prevTargetPos != targetPos) { // TODO: fix this
        path.clear();
        nextPathNode = -1;
        stillCounter = 0;
    }

    prevTargetPos = targetPos;

    if (path.empty() || nextPathNode < 0) {
        if (!dungeon->astar.FindPath(ownerPos, targetPos, path, owner->GetDiagonalMovementType())) {
            path.clear();
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
            ++stillCounter;
        }

        nextPathNode = path.size() - 1;
    }

    if (nextPathNode < 0)
        return;

    //+ Try Attacking if the target is close (for now only BasicAttack is used)
    int targetDist {static_cast<int>(glm::distance2(glm::vec2{ownerPos}, glm::vec2{targetPos}))};
    if (targetDist <= 2) {
        if (TurnManager::Instance().CanPerformNewAction(*owner))
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<BasicAttackAction>(owner, dynamic_cast<Unit*>(target), 10.0f, 0.15f));
        return;
    }

    auto& destination {path[nextPathNode]};
    if (dungeon->GetNode(destination.x, destination.y).unit) {
        glm::ivec2 destDir {destination - ownerPos};
        if (destDir.x > 1 || destDir.y > 1) { // Steps bigger than 1 tile/node
            owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
            path.clear();
            nextPathNode = -1;
            stillCounter = 0;
        }

        glm::ivec2 alt;
        bool foundAlternativePath {TryGetAlternativeMove(ownerPos, destination, nextPathNode - 1 < 0 ? nullptr : &path[nextPathNode - 1], owner, dungeon, alt)};

        if (foundAlternativePath) {
            auto moveAction {MakeOwned<MoveUnitAction>(owner, alt, 0.15f, dungeon)};
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

void ChaseState::OnExit() { }