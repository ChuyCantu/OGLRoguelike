#pragma once

#include "Core/Event.hpp"
#include "Game/AI/StateMachine.hpp"
#include "Game/TurnManager.hpp"

class Player : public Unit {
public:
    Player(class Scene* scene, const std::string& name = "Player");
    
    void Update() override;
    void OnCollisionEnter(const Collider& other) override;
    void OnCollisionStay(const Collider& other) override;
    void OnCollisionExit(const Collider& other) override;
    void DebugGUI() override;

private:
    StateMachine stateMachine;
};