#pragma once

#include "Game/AI/State.hpp"

#include <glm/vec2.hpp>
#include <vector>

class Unit;
class Dungeon;

class WanderState : public State {
public:
    WanderState(Unit* owner, Dungeon* dungeon);

    void OnEnter() override;
    void Update() override; 
    void OnExit() override; 

private:
    void MakeNewRandomPath(const glm::ivec2& startPos);

// private:
public:
    Unit* owner;
    Dungeon* dungeon;
    
    std::vector<glm::ivec2> path;
    int nextPathNode {-1};
    int stillCounter {0};
};