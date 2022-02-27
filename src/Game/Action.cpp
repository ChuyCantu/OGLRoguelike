#include "Action.hpp"

#include "Core/Log.hpp"
#include "Core/GameObject.hpp"
#include "TurnManager.hpp"

//+ Action =============================================
Action::Action(Battler* owner) 
    : owner{owner} {
}

//+ SkipAction ==========================================
SkipAction::SkipAction(Battler* owner) : Action{owner} {
    isCompleted = true;
    cost = owner->GetComponent<BattlerComponent>().GetSpeed();
}

void SkipAction::Update() {
    LOG_TRACE("{} skipped turn.", owner->name);
}

//+ MoveAction =========================================
MoveAction::MoveAction(Battler* owner, const glm::vec3& destination, float duration) : Action{owner}, destination{destination}, duration{duration} {
    cost = 100;

    auto& move {owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Subscribe("OnDestinationReached", &MoveAction::OnDestinationReached, this);
    move.onCancelation.Subscribe("OnMoveCanceled", &MoveAction::OnMoveCanceled, this);

    // LOG_TRACE("Move created");
}

MoveAction::MoveAction(Battler* owner, const glm::vec2& destination, float duration) : Action{owner}, destination{glm::vec3{destination.x, destination.y, 0.0f}}, duration{duration} {
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