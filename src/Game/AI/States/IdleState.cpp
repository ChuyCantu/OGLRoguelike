#include "IdleState.hpp"

#include "Game/Action.hpp"
#include "Game/TurnManager.hpp"

IdleState::IdleState(Unit* owner) : State{"IdleState"}, owner{owner} { }

#include "Core/Log.hpp"
void IdleState::OnEnter() {
    LOG_TRACE("Enter idle");
}

void IdleState::OnExit() {
    LOG_TRACE("Exit idle");
}

void IdleState::Update() {
    if (TurnManager::Instance().CanPerformNewAction(*owner)) {
        owner->GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(owner));
    }
}