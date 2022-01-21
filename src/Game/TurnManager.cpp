#include "TurnManager.hpp"

#include "Action.hpp"
#include "Core/Scene.hpp"

#include <algorithm>

//+ BattlerComponent =========================================
BattlerComponent::BattlerComponent(GameObject* gameobject, int attack, int health, int defense, int speed)
    : maxAttack{attack}, maxHealth{health}, maxDefense{defense}, maxSpeed{speed}, 
      attack{attack}, health{health}, defense{defense}, speed{speed} {
    this->gameobject = gameobject;
}

void BattlerComponent::TakeDamage(int dmg) {
    int finalDmg {std::max(dmg - defense, 0)};
    health -= finalDmg;
    if (health <= 0) {
        health = 0;
        onHealthDepleted.Invoke();
    }
}

void BattlerComponent::Heal(int hp) {
    health = std::min(health + hp, maxHealth);
}

bool BattlerComponent::IsAlive() const{
    return health > 0;
}

bool BattlerComponent::SetAction(Owned<Action> action) {
    if (action->GetCost() <= speed) {
        this->action = std::move(action);
        return true;
    }
    return false;
}

void BattlerComponent::ClearAction() {
    action.reset();
}

void BattlerComponent::ResetEnergy() {
    speed = maxSpeed;
}

void BattlerComponent::ConsumeEnergy(int value) {
    speed -= value;
}

//+ Battler =================================================
Battler::Battler(Scene* scene, const std::string& name) 
    : GameObject(scene, name) {
    TurnManager::GetInstance().AddBattler(this);
}

Battler::~Battler() {
    TurnManager::GetInstance().RemoveBattler(this);
}

//+ TurnManager =============================================
Owned<TurnManager> TurnManager::instance {nullptr};

TurnManager& TurnManager::GetInstance() {
    if (!instance) 
        instance = MakeOwned<TurnManager>();
    
    return *instance;
}

void TurnManager::Update() {
    if (battlers.empty()) {
        // Add new battlers if needed
        if (!addBattlerQueue.empty()) {
            for (Battler* battler : addBattlerQueue) {
                battlers.emplace_back(battler);
            }
            addBattlerQueue.clear();

            // Sort them with their speeds
            std::sort(battlers.begin(), battlers.end(), [](Battler* a, Battler* b) {
                return a->GetComponent<BattlerComponent>().GetSpeed() > b->GetComponent<BattlerComponent>().GetSpeed();
            });
        } 
        else {
            return;
        }
    }

    if (prepareNextBattler) {
        prepareNextBattler = false;

        // if (!addBattlerQueue.empty()) {
        //     std::sort(addBattlerQueue.begin(), addBattlerQueue.end(), [](Battler* a, Battler* b) {
        //         return a->GetComponent<BattlerComponent>().GetSpeed() > b->GetComponent<BattlerComponent>().GetSpeed();
        //     });

        //     for (Battler* battler : addBattlerQueue) {
        //         battlers.push_back(battler);
        //     }
        //     addBattlerQueue.clear();
        // }

        Action* action {battlers[currentBattler]->GetComponent<BattlerComponent>().GetAction()};
        if (action)
            action->OnEnd();

        do {
            UpdateCurrentBattler();
        } while (!battlers[currentBattler]->activeBattler && currentBattler != 0); 

        if (battlers.empty())
            return;

        // ++currentBattler;       
        // if (currentBattler >= battlers.size()) {
        //     currentBattler = 0;

        //     std::sort(battlers.begin(), battlers.end(), [](Battler* a, Battler* b) {
        //         return a->GetComponent<BattlerComponent>().GetSpeed() > b->GetComponent<BattlerComponent>().GetSpeed();
        //     });
        // }

        // action = battlers[currentBattler]->GetComponent<BattlerComponent>().GetAction();
        // if (action)
        //     action->OnStart();
    }

    Action* action {battlers[currentBattler]->GetComponent<BattlerComponent>().GetAction()};
    if ((action)) {
        if (!action->hasStarted) {
            action->OnStart();
            action->hasStarted = true;
        }

        if (action->IsCanceled()) {
            battlers[currentBattler]->GetComponent<BattlerComponent>().ClearAction();
        }
        else {
            action->Update();
            if (action->IsCompleted()) {
                action->GetOwner()->GetComponent<BattlerComponent>().ConsumeEnergy(action->GetCost());

                if (action->GetOwner()->GetComponent<BattlerComponent>().GetSpeed() <= 0) {
                    action->GetOwner()->GetComponent<BattlerComponent>().ResetEnergy();
                    prepareNextBattler = true;
                }

                battlers[currentBattler]->GetComponent<BattlerComponent>().ClearAction();
            }
        } 
    }   
}

Battler& TurnManager::AddBattler(Battler* battler) {
    return *addBattlerQueue.emplace_back(battler);
}

void TurnManager::RemoveBattler(Battler* battler) {
    battler->activeBattler = false;
    needCleaning = true;
    prepareNextBattler = true;
}

Battler* TurnManager::GetCurrentBattler() {
    if (!battlers.empty()) 
        return battlers[currentBattler];
    return nullptr;
}

void TurnManager::UpdateCurrentBattler() {
    ++currentBattler;       
    if (currentBattler >= battlers.size()) {
        currentBattler = 0;

        //! Delete removed battlers
        if (needCleaning) {
                needCleaning = false;
                battlers.erase(std::remove_if(battlers.begin(), battlers.end(), 
                    [](Battler* go) { 
                        return !go->activeBattler;
                    }
                ), battlers.end());
        }

        // Add new battlers if needed
        if (!addBattlerQueue.empty()) {
            for (Battler* battler : addBattlerQueue) {
                battlers.emplace_back(battler);
            }
            addBattlerQueue.clear();
        }

        // Sort them with their speeds
        std::sort(battlers.begin(), battlers.end(), [](Battler* a, Battler* b) {
            return a->GetComponent<BattlerComponent>().GetSpeed() > b->GetComponent<BattlerComponent>().GetSpeed();
        });
    }
}

void TurnManager::Clear() {
    battlers.clear();
    addBattlerQueue.clear();
}