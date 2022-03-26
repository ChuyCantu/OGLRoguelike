#pragma once

#include "Game/TurnManager.hpp"
#include "Game/AI/StateMachine.hpp"

class Enemy : public Unit {
public:
    Enemy(class Scene* scene, const std::string& name = "Enemy");

    void Update() override;
    // void OnCollisionEnter(const Collider& other) override;
    // void OnCollisionStay(const Collider& other) override;
    // void OnCollisionExit(const Collider& other) override;
    void DebugGUI() override;

public:
    entt::entity playerRef {entt::null};
    class Dungeon* dungeon;

private:
    StateMachine stateMachine;
};