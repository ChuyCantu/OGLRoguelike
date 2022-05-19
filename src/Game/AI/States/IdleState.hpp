#pragma once

#include "Game/AI/State.hpp"

class IdleState : public State {
public:
    IdleState(class Unit* owner);

    void OnEnter() override;
    void OnExit() override;
    void Update() override;

private:
    class Unit* owner;
};