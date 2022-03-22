#pragma once

#include "Game/AI/StateMachine.hpp"
#include "Core/Event.hpp"
#include "Game/TurnManager.hpp"

class UnitPlayer : public Unit {
public:
    UnitPlayer(class Scene* scene, const std::string& name = "PlayerUnit");
    ~UnitPlayer() override;

    void Update() override;
    void OnCollisionEnter(const Collider& other) override;
    void OnCollisionStay(const Collider& other) override;
    void OnCollisionExit(const Collider& other) override;
    void DebugGUI() override;

private:
    StateMachine stateMachine;
};

class UnitEnemy : public Unit {
public:
    UnitEnemy(class Scene* scene, const std::string& name = "EnemyUnit");
    ~UnitEnemy() override;
    
    void Update() override;
};