#ifndef __BATTLERS_H__
#define __BATTLERS_H__

#include "AI/StateMachine.hpp"
#include "Core/Event.hpp"
#include "TurnManager.hpp"

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

#endif // __BATTLERS_H__