#ifndef __ACTION_H__
#define __ACTION_H__

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Battler;

class Action {
public:
    Action(Battler* owner);
    virtual ~Action() {}

    virtual void OnStart() {};
    virtual void OnEnd()   {};
    virtual void Update()  {};

    bool IsCompleted() const { return isCompleted; };
    bool IsCanceled() const { return canceled; }

    Battler* GetOwner() { return owner; }
    int GetCost() const { return cost; }

protected:
    Battler* owner;
    int cost         {0};
    bool isCompleted {false};
    bool canceled    {false};
    bool hasStarted  {false};

    friend class TurnManager;
};

class SkipAction : public Action {
public:
    SkipAction(Battler* owner);

    void Update() override;
};

class MoveAction : public Action {
public:
    MoveAction(Battler* owner, const glm::vec3& destination, float duration);
    MoveAction(Battler* owner, const glm::vec2& destination, float duration);
    ~MoveAction() override;
   
    void OnStart() override;

    void OnDestinationReached();
    void OnMoveCanceled();

private:
    glm::vec3 destination;
    float duration;
};

#endif // __ACTION_H__