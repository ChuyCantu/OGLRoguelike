#pragma once

#include "Core/Event.hpp"
#include "Game/Algorithms.hpp"

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Unit;

class Action {
public:
    Action(Unit* owner);
    virtual ~Action() {}

    virtual void OnStart() {};
    virtual void OnEnd()   {};
    virtual void Update()  {};

    void Cancel();

    bool IsCompleted() const { return isCompleted; };
    /** Actions that are completed asynchronously consume energy as soon as IsCompletedAsync returns true
     *  so fail conditions must be done BEFORE the completeAsync flag is set to true;
     */
    bool IsCompletedAsync() const { return completeAsync; }
    bool IsCanceled() const { return canceled; }

    Unit* GetOwner() { return owner; }
    int GetCost() const { return cost; }

protected:
    Unit* owner;
    int cost           {0};
    bool isCompleted   {false};
    bool completeAsync {false};
    bool canceled      {false};
    bool hasStarted    {false};

    friend class TurnManager;
};

class SkipAction : public Action {
public:
    SkipAction(Unit* owner);

    void Update() override;
};

class MoveAction : public Action {
public:
    MoveAction(Unit* owner, const glm::vec3& destination, float duration);
    MoveAction(Unit* owner, const glm::vec2& destination, float duration);
    ~MoveAction() override;
   
    void OnStart() override;

    void OnDestinationReached();
    void OnMoveCanceled();

private:
    glm::vec3 destination;
    float duration;
};

class MoveUnitAction : public Action {
public:
    MoveUnitAction(Unit* owner, const glm::ivec2& destination, float duration, class Dungeon* dungeon);
    ~MoveUnitAction() override;
   
    void OnStart() override;

    void OnDestinationReached();
    void OnMoveCanceled();

public:
    Event<void()> onMoveActionCanceled;
    Event<void()> onMoveActionCompleted;

private:
    glm::ivec2 tileDest;
    float duration;
    class Dungeon* dungeon;
};

class BasicAttackAction : public Action {
public:
    BasicAttackAction(Unit* owner, Unit* target, float movementDistance, float animDuration);

    void OnStart() override;
    void Update() override;
    void OnEnd() override;

private:
    void Attack();

private:
    Unit* target;
    float movementDistance;
    float animDuration;

    glm::vec2 srcPosition;
    glm::vec2 destination;
    float attackTime;
    float repositionTime;
    float timer {0.0f};
    bool reachedAttackPosition {false};
};