#ifndef __TURNMANAGER_H__
#define __TURNMANAGER_H__

#include "Core/Components.hpp"
#include "Core/GameObject.hpp"
#include "Core/Event.hpp"

struct BattlerComponent : public Component {
    BattlerComponent(GameObject* gameobject, int attack, int health, int defense, int speed);

    void TakeDamage(int dmg);
    void Heal(int hp);
    bool IsAlive() const;

    bool SetAction(Owned<class Action> action);
    void ClearAction();
    void ResetEnergy();
    void ConsumeEnergy(int value);

    void SetMaxAttack(int maxAtk)  { maxAttack = maxAtk; }
    void SetMaxHealth(int maxHp)   { maxHealth = maxHp; }
    void SetMaxDefense(int maxDef) { maxDefense = maxDef; }
    void SetMaxSpeed(int maxSpd)   { maxSpeed = maxSpd; }
    void SetAttack(int atk)        { attack = atk; }
    void SetHealth(int hp)         { health = hp; }
    void SetDefense(int def)       { defense = def; }
    void SetSpeed(int spd)         { speed = spd; }

    int GetMaxAttack() const  { return maxAttack; }
    int GetMaxHealth() const  { return maxHealth; }
    int GetMaxDefense() const { return maxDefense; }
    int GetMaxSpeed() const   { return maxSpeed; }
    int GetAttack() const     { return attack; }
    int GetHealth() const     { return health; }
    int GetDefense() const    { return defense; }
    int GetSpeed() const      { return speed; }

    Action* GetAction() { return action.get(); }

    Event<void()> onHealthDepleted;

private:
    int maxAttack  {0};
    int maxHealth  {0};
    int maxDefense {0};
    int maxSpeed   {0};

    int attack  {0};
    int health  {0};
    int defense {0};
    int speed   {0};

    Owned<class Action> action {nullptr};
};

// Battler is any entity that is affected by the turn based move system
class Battler : public GameObject {
public:
    Battler(class Scene* scene, const std::string& name = "Battler");
    ~Battler() override;

private:
    bool activeBattler = true;

    friend class TurnManager;
};

class TurnManager {
public:
 static TurnManager& Instance();

 void Update();
 Battler& AddBattler(Battler* battler);
 void RemoveBattler(Battler* battler);
 Battler* GetCurrentBattler();
 bool CanPerformNewAction(Battler& battler);
 void Clear();

private:
    void UpdateCurrentBattler();

private:
    static Owned<TurnManager> instance;

#ifdef ALTERNATIVE
    bool prepareNextBattler {false};
#endif
    // bool needCleaning       {false};
    uint32_t currentBattlerIdx {0};
    std::vector<Battler*> battlers;
    std::vector<Battler*> addBattlerQueue;
};

#endif // __TURNMANAGER_H__