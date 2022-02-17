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
    TurnManager::Instance().AddBattler(this);
}

Battler::~Battler() {
    TurnManager::Instance().RemoveBattler(this);
}

//+ TurnManager =============================================
Owned<TurnManager> TurnManager::instance {nullptr};

TurnManager& TurnManager::Instance() {
    if (!instance) 
        instance = MakeOwned<TurnManager>();
    
    return *instance;
}

void TurnManager::Update() {
    if (battlers.empty() || currentBattlerIdx >= battlers.size()) {
        currentBattlerIdx = 0;

        // Add new battlers if needed
        if (addBattlerQueue.empty()) 
            return; 

        for (Battler* battler : addBattlerQueue) {
            battlers.emplace_back(battler);
        }
        addBattlerQueue.clear();

        // Sort them with their speeds
        std::sort(battlers.begin(), battlers.end(), [](Battler* a, Battler* b) {
            return a->GetComponent<BattlerComponent>().GetMaxSpeed() > b->GetComponent<BattlerComponent>().GetMaxSpeed();
        });

#ifdef ALTERNATIVE
        if (!addBattlerQueue.empty()) {
            for (Battler* battler : addBattlerQueue) {
                battlers.emplace_back(battler);
            }
            addBattlerQueue.clear();

            // Sort them with their speeds
            std::sort(battlers.begin(), battlers.end(), [](Battler* a, Battler* b) {
                return a->GetComponent<BattlerComponent>().GetSpeed() > b->GetComponent<BattlerComponent>().GetSpeed();
            });
        } else
            return;
#endif
    }

    auto& currentBattler{battlers[currentBattlerIdx]};
    if (!currentBattler->activeBattler) {
        do {
            UpdateCurrentBattler();
        } while (!battlers[currentBattlerIdx]->activeBattler && currentBattlerIdx != 0);

        if (battlers.empty())
            return;
    }

    auto& battlerComponent{currentBattler->GetComponent<BattlerComponent>()};
    Action* action{battlerComponent.GetAction()};
    if (action) {
        if (!action->hasStarted) {
            action->OnStart();
            action->hasStarted = true;
            // battlerComponent.ConsumeEnergy(action->GetCost()); //!
        }

        if (action->IsCanceled()) {
            // battlerComponent.SetSpeed(battlerComponent.GetSpeed() + action->GetCost()); //!
            battlerComponent.ClearAction();
        } 
        else {
            action->Update();
            if (action->IsCompleted()) {
                battlerComponent.ConsumeEnergy(action->GetCost());  //!

                action->OnEnd();
                battlerComponent.ClearAction();

                if (battlerComponent.GetSpeed() <= 0) { // TODO: or <= minActionCost 
                    battlerComponent.ResetEnergy();

                    do {
                        UpdateCurrentBattler();
                    } while (!battlers[currentBattlerIdx]->activeBattler && currentBattlerIdx != 0);
                }
            }
        }
    }

#ifdef ALTERNATIVE
    auto& currentBattler {battlers[currentBattlerIdx]};
    if (!currentBattler->activeBattler)
        prepareNextBattler = true;

    if (prepareNextBattler) {
        prepareNextBattler = false;

        do {
            UpdateCurrentBattler();
        } while (!battlers[currentBattlerIdx]->activeBattler && currentBattlerIdx != 0); 

        if (battlers.empty())
            return;
    }

    auto& battlerComponent {currentBattler->GetComponent<BattlerComponent>()};
    Action* action {battlerComponent.GetAction()};
    if ((action)) {
        if (!action->hasStarted) {
            action->OnStart();
            action->hasStarted = true;
        }

        if (action->IsCanceled()) {
            battlerComponent.ClearAction();
        }
        else {
            action->Update();
            if (action->IsCompleted()) {
                battlerComponent.ConsumeEnergy(action->GetCost());

                if (battlerComponent.GetSpeed() <= 0) {
                    battlerComponent.ResetEnergy();
                    prepareNextBattler = true;
                }

                action->OnEnd();
                battlerComponent.ClearAction();
            }
        } 
    }   
#endif
}

Battler& TurnManager::AddBattler(Battler* battler) {
    return *addBattlerQueue.emplace_back(battler);
}

void TurnManager::RemoveBattler(Battler* battler) {
    battler->activeBattler = false;
    // needCleaning = true;
    // prepareNextBattler = true;

    battlers.erase(std::remove_if(battlers.begin(), battlers.end(), 
        [&battler] (Battler* b) {
            return b == battler;
        }
    ), battlers.end());
}

Battler* TurnManager::GetCurrentBattler() {
    if (!battlers.empty()) 
        return battlers[currentBattlerIdx];
    return nullptr;
}

void TurnManager::UpdateCurrentBattler() {
    ++currentBattlerIdx;       
    if (currentBattlerIdx >= battlers.size()) {
        currentBattlerIdx = 0;

        // // //! Delete removed battlers
        // if (needCleaning) {
        //         needCleaning = false;
        //         battlers.erase(std::remove_if(battlers.begin(), battlers.end(), 
        //             [](Battler* go) { 
        //                 return !go->activeBattler;
        //             }
        //         ), battlers.end());
        // }

        // Add new battlers if needed
        if (!addBattlerQueue.empty()) {
            for (Battler* battler : addBattlerQueue) {
                battlers.emplace_back(battler);
            }
            addBattlerQueue.clear();
        }

        if (!battlers.empty()) {
            // Sort them with their speeds
            std::sort(battlers.begin(), battlers.end(), [](Battler* a, Battler* b) {
                return a->GetComponent<BattlerComponent>().GetMaxSpeed() > b->GetComponent<BattlerComponent>().GetMaxSpeed();
            });
        }
    }
}

bool TurnManager::CanPerformNewAction(Battler& battler) {
    Battler* current {GetCurrentBattler()};
    if (current) {
        return *current == battler && !battler.GetComponent<BattlerComponent>().GetAction();
    }
    return false;
}

void TurnManager::Clear() {
    battlers.clear();
    addBattlerQueue.clear();
}