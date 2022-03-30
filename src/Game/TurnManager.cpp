#include "TurnManager.hpp"

#include "Action.hpp"
#include "Constants.hpp"
#include "Core/Scene.hpp"
#include "DungeonGen/Dungeon.hpp"

#include <algorithm>

//+ UnitComponent =========================================
UnitComponent::UnitComponent(int attack, int health, int defense, int speed)
    : maxAttack{attack}, maxHealth{health}, maxDefense{defense}, maxSpeed{speed}, 
      attack{attack}, health{health}, defense{defense}, speed{speed} { }

void UnitComponent::TakeDamage(int dmg) {
    int finalDmg {std::max(dmg - defense, 0)};
    health -= finalDmg;
    if (health <= 0) {
        health = 0;
        onHealthDepleted.Invoke();
    }
}

void UnitComponent::Heal(int hp) {
    health = std::min(health + hp, maxHealth);
}

bool UnitComponent::IsAlive() const{
    return health > 0;
}

bool UnitComponent::SetAction(Owned<Action> action) {
    if (action->GetCost() <= speed) {
        this->action = std::move(action);
        return true;
    }
    return false;
}

void UnitComponent::ClearAction() {
    action.reset();
}

Owned<Action> UnitComponent::GetActionOwnership() {
    Owned<Action> result {std::move(action)};
    action = nullptr;
    return result;
}

void UnitComponent::ResetEnergy() {
    speed = maxSpeed;
}

void UnitComponent::ConsumeEnergy(int value) {
    speed -= value;
}

//+ Unit =================================================
Unit::Unit(Scene* scene, const std::string& name) 
    : GameObject(scene, name) {
    TurnManager::Instance().AddUnit(this);

    AddCommponent<MoveComponent>().Teleport(glm::vec3{0.f, 0.f, 0.0f});
}   

Unit::~Unit() {
    TurnManager::Instance().RemoveUnit(this);

    // Free node that was previously occupied by this unit
    if (dungeon) {
        auto& pos {GetComponent<Transform>().GetPosition()};
        auto node{dungeon->TryGetNode(pos.x / TILE_SIZE, pos.y / TILE_SIZE)};
        if (node) node->unit = nullptr;
    }
}

void Unit::SetStartPosition(const glm::ivec2 position) {
    GetComponent<MoveComponent>().Teleport(glm::vec3{position.x, position.y, 0} * TILE_SIZEF);

    if (dungeon) {
        auto node{dungeon->TryGetNode(position.x, position.y)};
        if (node) node->unit = this;
    }
}

//+ TurnManager =============================================
Owned<TurnManager> TurnManager::instance {nullptr};

TurnManager& TurnManager::Instance() {
    if (!instance) 
        instance = MakeOwned<TurnManager>();
    
    return *instance;
}

void TurnManager::Update() {
    if (units.empty() || currentUnitIdx >= units.size()) {
        currentUnitIdx = 0;

        // Add new units if needed
        if (addUnitQueue.empty()) 
            return; 

        for (Unit* unit : addUnitQueue) {
            units.emplace_back(unit);
        }
        addUnitQueue.clear();

        // Sort them with their speeds
        std::sort(units.begin(), units.end(), [](Unit* a, Unit* b) {
            return a->GetComponent<UnitComponent>().GetMaxSpeed() > b->GetComponent<UnitComponent>().GetMaxSpeed();
        });
    }

    auto& currentUnit{units[currentUnitIdx]};
    if (!currentUnit->activeUnit) {
        do {
            UpdateCurrentUnit();
        } while (!units[currentUnitIdx]->activeUnit && currentUnitIdx != 0);

        if (units.empty())
            return;
    }

    auto& unitComponent{currentUnit->GetComponent<UnitComponent>()};
    Action* action {unitComponent.GetAction()};
    if (action) {
        if (!action->hasStarted) {
            action->OnStart();
            action->hasStarted = true;
            // unitComponent.ConsumeEnergy(action->GetCost()); //!
        }

        if (action->IsCanceled()) {
            // unitComponent.SetSpeed(unitComponent.GetSpeed() + action->GetCost()); //!
            unitComponent.ClearAction();
        } 
        else {
            action->Update();

            if (action->IsCompleted() || action->IsCompletedAsync()) {
                unitComponent.ConsumeEnergy(action->GetCost());  //!

                if (action->IsCompletedAsync()) 
                    asyncActions.push_back(std::move(unitComponent.GetActionOwnership()));
                else {
                    action->OnEnd();
                    unitComponent.ClearAction();
                }

                if (unitComponent.GetSpeed() <= 0) { // TODO: or <= minActionCost 
                    unitComponent.ResetEnergy();

                    do {
                        UpdateCurrentUnit();
                    } while (!units[currentUnitIdx]->activeUnit && currentUnitIdx != 0);
                }
            }
        }
    }

    if (!asyncActions.empty()) {
        // Async updates
        for (auto& asyncAction : asyncActions) {
            asyncAction->Update();

            if (asyncAction->IsCompleted()) {
                asyncAction->OnEnd();
            }
        }

        // Delete completed async actions
        asyncActions.erase(std::remove_if(asyncActions.begin(), asyncActions.end(),
            [](Owned<Action>& a) {
                return a->IsCompleted() || a->IsCanceled();
            }),
        asyncActions.end());
    }
}

Unit& TurnManager::AddUnit(Unit* unit) {
    return *addUnitQueue.emplace_back(unit);
}

void TurnManager::RemoveUnit(Unit* unit) {
    unit->activeUnit = false;
    // needCleaning = true;
    // prepareNextUnit = true;

    units.erase(std::remove_if(units.begin(), units.end(), 
        [&unit] (Unit* b) {
            return b == unit;
        }
    ), units.end());
}

Unit* TurnManager::GetCurrentUnit() {
    if (!units.empty()) 
        return units[currentUnitIdx];
    return nullptr;
}

void TurnManager::UpdateCurrentUnit() {
    ++currentUnitIdx;       
    if (currentUnitIdx >= units.size()) {
        currentUnitIdx = 0;

        // // //! Delete removed units
        // if (needCleaning) {
        //         needCleaning = false;
        //         units.erase(std::remove_if(units.begin(), units.end(), 
        //             [](Unit* go) { 
        //                 return !go->activeUnit;
        //             }
        //         ), units.end());
        // }

        // Add new units if needed
        if (!addUnitQueue.empty()) {
            for (Unit* unit : addUnitQueue) {
                units.emplace_back(unit);
            }
            addUnitQueue.clear();
        }

        if (!units.empty()) {
            // Sort them with their speeds
            std::sort(units.begin(), units.end(), [](Unit* a, Unit* b) {
                return a->GetComponent<UnitComponent>().GetMaxSpeed() > b->GetComponent<UnitComponent>().GetMaxSpeed();
            });
        }
    }
}

bool TurnManager::CanPerformNewAction(Unit& unit) {
    Unit* current {GetCurrentUnit()};
    if (current) {
        if (current->tag == "Player" && !asyncActions.empty())
            return false;

        return *current == unit && !unit.GetComponent<UnitComponent>().GetAction();
    }
    return false;
}

void TurnManager::Clear() {
    units.clear();
    addUnitQueue.clear();
    asyncActions.clear();
}