#pragma once

#include "Game/AI/State.hpp"

#include <entt/entity/entity.hpp>
#include <glm/vec2.hpp>
#include <vector>

class Unit;
class Dungeon;

class ChaseState : public State {
public:
    // target must be an instance of a Unit or must have a UnitComponent attached to it
    ChaseState(Unit* owner, entt::entity targetRef, Dungeon* dungeon);

    void OnEnter() override;
    void Update() override;
    void OnExit() override;

private:
    Unit* owner;
    entt::entity targetRef;
    Dungeon* dungeon;

public: // TODO: Remove this
    std::vector<glm::ivec2> path;
    int nextPathNode {-1};
    int stillCounter {0};
    glm::ivec2 prevTargetPos;
};