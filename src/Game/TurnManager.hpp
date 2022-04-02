#pragma once

#include "Game/Algorithms.hpp"
#include "Core/Components.hpp"
#include "Core/GameObject.hpp"
#include "Core/Event.hpp"

class Action;

struct UnitComponent : public Component {
    UnitComponent(int attack, int health, int defense, int speed);

    void TakeDamage(int dmg);
    void Heal(int hp);
    bool IsAlive() const;

    bool SetAction(Owned<Action> action);
    void ClearAction();
    Owned<Action> GetActionOwnership();
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

    void UpdatePosition(const glm::ivec2& position) { this->position = position; }
    const glm::ivec2& GetPosition() const { return position; }

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

    glm::ivec2 position {0, 0}; // in dungeon

    Owned<Action> action {nullptr};
};

// Unit is any entity that is affected by the turn based move system
class Unit : public GameObject {
public:
    Unit(class Scene* scene, const std::string& name = "Unit");
    ~Unit() override;

    void SetStartPosition(const glm::ivec2 position);
    
    DiagonalMovement GetDiagonalMovementType() const { return diagonalMovementType; }

public:
    class Dungeon* dungeon;

protected:
    DiagonalMovement diagonalMovementType {DiagonalMovement::OnlyWhenNoObstacles};

private:
    bool activeUnit = true;


    friend class TurnManager;
};

class TurnManager {
public:
    static TurnManager& Instance();

    void Update();
    Unit& AddUnit(Unit* unit);
    void RemoveUnit(Unit* unit);
    Unit* GetCurrentUnit();
    bool CanPerformNewAction(Unit& unit);
    void Clear();
    void SkipUnitsBeforePlayer();

private:
    void UpdateCurrentUnit();

private:
    static Owned<TurnManager> instance;

#ifdef ALTERNATIVE
    bool prepareNextUnit {false};
#endif
    // bool needCleaning       {false};
    uint32_t currentUnitIdx {0};
    std::vector<Unit*> units;
    std::vector<Unit*> addUnitQueue;
    std::vector<Owned<Action>> asyncActions;
};