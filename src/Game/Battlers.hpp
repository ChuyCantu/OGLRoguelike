#ifndef __BATTLERS_H__
#define __BATTLERS_H__

#include "TurnManager.hpp"

class BattlerPlayer : public Battler {
public:
    BattlerPlayer(class Scene* scene, const std::string& name = "PlayerBattler");
    ~BattlerPlayer() override;

    void Update() override;
    void OnCollisionEnter(const Collider& other) override;
};

class BattlerEnemy : public Battler {
public:
    BattlerEnemy(class Scene* scene, const std::string& name = "EnemyBattler");
    ~BattlerEnemy() override;
    
    void Update() override;
};

#endif // __BATTLERS_H__