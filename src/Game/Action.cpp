#include "Action.hpp"

#include "Constants.hpp"
#include "Core/Log.hpp"
#include "Core/GameObject.hpp"
#include "DungeonGen/Dungeon.hpp"
#include "TurnManager.hpp"


//+ Action =============================================
Action::Action(Unit* owner) 
    : owner{owner} {
}

void Action::Cancel() {
    canceled = true;
}

//+ SkipAction ==========================================
SkipAction::SkipAction(Unit* owner) : Action{owner} {
    isCompleted = true;
    cost = owner->GetComponent<UnitComponent>().GetSpeed();
}

void SkipAction::Update() {
    LOG_TRACE("{} skipped turn.", owner->name);
}

//+ MoveAction =========================================
MoveAction::MoveAction(Unit* owner, const glm::vec3& destination, float duration) : Action{owner}, destination{destination}, duration{duration} {
    cost = 100;

    auto& move {owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Subscribe("OnDestinationReached", &MoveAction::OnDestinationReached, this);
    move.onCancelation.Subscribe("OnMoveCanceled", &MoveAction::OnMoveCanceled, this);

    // LOG_TRACE("Move created");
}

MoveAction::MoveAction(Unit* owner, const glm::vec2& destination, float duration) : Action{owner}, destination{glm::vec3{destination.x, destination.y, 0.0f}}, duration{duration} {
    cost = 100;

    auto& move{owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Subscribe("OnDestinationReached", &MoveAction::OnDestinationReached, this);
    move.onCancelation.Subscribe("OnMoveCanceled", &MoveAction::OnMoveCanceled, this);

    // LOG_TRACE("Move created");
}

MoveAction::~MoveAction() {
    auto& move{owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Unsubscribe("OnDestinationReached", this);
    move.onCancelation.Unsubscribe("OnMoveCanceled", this);

    // LOG_TRACE("Move deleted");
}

void MoveAction::OnStart()  {
    owner->GetComponent<MoveComponent>().Move(destination, duration);
    
    // LOG_TRACE("{}", owner->name);
}

void MoveAction::OnDestinationReached() {
    isCompleted = true;
}

void MoveAction::OnMoveCanceled() {
    canceled = true;
}

//+ MoveUnitAction =========================================
MoveUnitAction::MoveUnitAction(Unit* owner, const glm::vec3& destination, float duration, Dungeon* dungeon, DiagonalMovement diagonalMovement) 
    : Action{owner}, destination{destination}, duration{duration}, dungeon{dungeon}, diagonalMovement{diagonalMovement} {
    cost = 100;

    auto& move {owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Subscribe("OnDestinationReached", &MoveUnitAction::OnDestinationReached, this);
    move.onCancelation.Subscribe("OnMoveCanceled", &MoveUnitAction::OnMoveCanceled, this);
}

MoveUnitAction::MoveUnitAction(Unit* owner, const glm::vec2& destination, float duration, Dungeon* dungeon, DiagonalMovement diagonalMovement) 
    : Action{owner}, destination{glm::vec3{destination.x, destination.y, 0.0f}}, duration{duration}, dungeon{dungeon}, diagonalMovement{diagonalMovement} {
    cost = 100;

    auto& move{owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Subscribe("OnDestinationReached", &MoveUnitAction::OnDestinationReached, this);
    move.onCancelation.Subscribe("OnMoveCanceled", &MoveUnitAction::OnMoveCanceled, this);
}

MoveUnitAction::~MoveUnitAction() {
    auto& move{owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Unsubscribe("OnDestinationReached", this);
    move.onCancelation.Unsubscribe("OnMoveCanceled", this);
}

void MoveUnitAction::OnStart()  {
    auto& destinationNode {dungeon->GetNode(destination.x / TILE_SIZE, destination.y / TILE_SIZE)};
    if (destinationNode.unit) {  // The destination is occupied by another unit
        // canceled = true;
        
        // For now, if the movement is not possible, just skip this turn
        isCompleted = true;
        cost = owner->GetComponent<UnitComponent>().GetSpeed();
        return;
    }

    auto& currPos {owner->GetComponent<Transform>().GetPosition()};
    dungeon->GetNode(static_cast<int>(currPos.x) / TILE_SIZE, static_cast<int>(currPos.y) / TILE_SIZE).unit = nullptr;
    destinationNode.unit = owner;
    owner->GetComponent<MoveComponent>().Move(destination, duration);
    completeAsync = true;
}

void MoveUnitAction::OnDestinationReached() {
    isCompleted = true;
}

void MoveUnitAction::OnMoveCanceled() {
    canceled = true;
    auto& destinationNode {dungeon->GetNode(destination.x / TILE_SIZE, destination.y / TILE_SIZE)};
    if (destinationNode.unit == owner) {
        destinationNode.unit = nullptr;
        
        auto& srcPos {owner->GetComponent<MoveComponent>().GetSrcPosition()};
        auto& srcNode {dungeon->GetNode(static_cast<int>(srcPos.x) / TILE_SIZE, static_cast<int>(srcPos.y) / TILE_SIZE)};
        srcNode.unit = owner;
    }
    onMoveActionCanceled.Invoke();
}