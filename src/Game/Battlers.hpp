#ifndef __BATTLERS_H__
#define __BATTLERS_H__

#include "TurnManager.hpp"
#include "Core/Event.hpp"

class BattlerPlayer : public Battler {
public:
    BattlerPlayer(class Scene* scene, const std::string& name = "PlayerBattler");
    ~BattlerPlayer() override;

    void Update() override;
    void OnCollisionEnter(const Collider& other) override;
    void DebugGUI() override;

    //! Debug
    class Label* testLabel;
    class Slider* sliderTest {nullptr};
    class Slider* sliderTest2 {nullptr};
    Event<void(BattlerPlayer*)> onDestroy;
};

class BattlerEnemy : public Battler {
public:
    BattlerEnemy(class Scene* scene, const std::string& name = "EnemyBattler");
    ~BattlerEnemy() override;
    
    void Update() override;
};

#endif // __BATTLERS_H__