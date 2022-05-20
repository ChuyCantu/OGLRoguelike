#pragma once

#include "Game/TurnManager.hpp"
#include "Game/AI/StateMachine.hpp"

class Enemy : public Unit {
public:
    Enemy(class Scene* scene, const std::string& name = "Enemy");

    void Start() override;
    void Update() override;
    void OnCollisionEnter(const Collider& other) override;
    void DebugGUI() override;

public:
    entt::entity playerRef {entt::null};

protected:
    StateMachine stateMachine;
    // std::vector<glm::ivec2> currentPath;
    // uint32_t nextPathNode {0};
    bool isPlayerInRange {false};

    Ref<class WanderState> _wanderState{nullptr};
    Ref<class ChaseState> _chaseState{nullptr};
};