#include "Enemy.hpp"

#include "Game/Action.hpp"
#include "Game/Constants.hpp"
#include "Game/DungeonGen/Dungeon.hpp"
#include "Input/Input.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Core/Engine.hpp"
#include "Rendering/Renderer.hpp"
#include "Game/AI/States/IdleState.hpp"
#include "Game/AI/States/WanderState.hpp"

#include <imgui.h>

Enemy::Enemy(Scene* scene, const std::string& name) : Unit{scene, name} {
    tag = "Enemy";
    auto& sr{AddComponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("slime0_spritesheet"), glm::ivec2{16, 16}, glm::ivec2{TILE_SIZE, TILE_SIZE}), ColorNames::white, 0)};
    sr.pivot = glm::vec2{0.0f, -0.25f};

    auto& animator{AddComponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("slime0_spritesheet"), 0.25f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("slime1_spritesheet"), 0.25f});

    AddComponent<Collider>();
    // AddComponent<MoveComponent>().Teleport(glm::vec3{0.f, 0.f, 0.0f});
    AddComponent<UnitComponent>(1, 10, 0, 100);
}

void Enemy::Start() {
    //+ State Machine
    // Ref<WanderState> wanderState {MakeRef<WanderState>(this, dungeon)};
    Ref<WanderState> wanderState = MakeRef<WanderState>(this, dungeon);
    Ref<IdleState> idleState{MakeRef<IdleState>(this)};

    stateMachine.AddState(wanderState);
    stateMachine.AddState(idleState);

    stateMachine.AddTransition(idleState, wanderState, [this]() { return !this->isPlayerInRange; });
    stateMachine.AddTransition(wanderState, idleState, [this]() { return this->isPlayerInRange; });

    stateMachine.SetState(idleState);

    _wanderState = wanderState;
}

void Enemy::Update() {
    auto& ipos {GetComponent<UnitComponent>().GetPosition()};
    auto& fovNode {dungeon->fov.GetNode(ipos.x, ipos.y)};
    float light {fovNode.lightLevel};
    auto& spriteRenderer {GetComponent<SpriteRenderer>()};
    spriteRenderer.color = Color{light, light, light};
    spriteRenderer.enabled = fovNode.visible;
    // isPlayerInRange = fovNode.visible;

    stateMachine.Update();

    //! Deprecated:
    // auto player {scene->FindGameObject(playerRef)};
    // int distanceToPlayer {INT_MAX};
    // if (player) {
    //     distanceToPlayer = static_cast<int>(glm::distance(glm::vec2{player->GetComponent<UnitComponent>().GetPosition()}, glm::vec2{GetComponent<UnitComponent>().GetPosition()}));
    // }
    // isPlayerClose = distanceToPlayer <= 0;

    // if (TurnManager::Instance().CanPerformNewAction(*this)) {
    //     // GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
    //     if (currentPath.empty() || nextPathNode == 0) {
    //         auto& transform{GetComponent<Transform>()};
    //         glm::ivec2 pos{transform.GetPosition().x, transform.GetPosition().y};
    //         glm::ivec2 pathStart{pos / TILE_SIZE};
    //         if (pos.x < 0) --pathStart.x;
    //         if (pos.y < 0) --pathStart.y;

    //         glm::ivec2 pathEnd{playerPos / TILE_SIZE};
    //         if (playerPos.x < 0) --pathEnd.x;
    //         if (playerPos.y < 0) --pathEnd.y;

    //         if (!dungeon->astar.FindPath(pathStart, pathEnd, currentPath, DiagonalMovement::OnlyWhenNoObstacles)) {
    //             currentPath.clear();
    //             GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
    //         }
    //         nextPathNode = currentPath.size() - 1;
    //     }

    //     GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, currentPath[--nextPathNode] * TILE_SIZE, 0.15f, dungeon));
    // }


    //! Debug
    if (_wanderState && !_wanderState->path.empty()) {
        for (size_t i{0}; i < _wanderState->path.size() - 1; ++i) {
            glm::ivec2 p{_wanderState->path[i + 1] - _wanderState->path[i]};
            // if (p.x != 0 && p.y != 0) {  // diagonal move
            //     scene->GetEngine()->GetRenderer()->DrawLine2D(_wanderState->path[i] * TILE_SIZE + 8, _wanderState->path[i + 1] * TILE_SIZE + 8, Color2Vec3(ColorNames::white));

            //     // adjacent nodes
            //     scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{_wanderState->path[i].x, _wanderState->path[i + 1].y} * TILE_SIZEF, glm::vec2{_wanderState->path[i].x, _wanderState->path[i + 1].y} * TILE_SIZEF + TILE_SIZEF, Color2Vec3(ColorNames::cyan));
            //     scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{_wanderState->path[i].x, _wanderState->path[i + 1].y} * TILE_SIZEF + glm::vec2{0.f, TILE_SIZEF}, glm::vec2{_wanderState->path[i].x, _wanderState->path[i + 1].y} * TILE_SIZEF + glm::vec2{TILE_SIZEF, 0.f}, Color2Vec3(ColorNames::cyan));
            //     scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{_wanderState->path[i + 1].x, _wanderState->path[i].y} * TILE_SIZEF, glm::vec2{_wanderState->path[i + 1].x, _wanderState->path[i].y} * TILE_SIZEF + TILE_SIZEF, Color2Vec3(ColorNames::cyan));
            //     scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{_wanderState->path[i + 1].x, _wanderState->path[i].y} * TILE_SIZEF + glm::vec2{0.f, TILE_SIZEF}, glm::vec2{_wanderState->path[i + 1].x, _wanderState->path[i].y} * TILE_SIZEF + glm::vec2{TILE_SIZEF, 0.f}, Color2Vec3(ColorNames::cyan));
            // } else
                scene->GetEngine()->GetRenderer()->DrawLine2D(_wanderState->path[i] * 16 + 8, _wanderState->path[i + 1] * 16 + 8, Color2Vec3(ColorNames::white));
        }
    }
}

void Enemy::OnCollisionEnter(const Collider& other) {
    // auto& unitComp {GetComponent<UnitComponent>()};
    // unitComp.GetAction()->Cancel();
    // unitComp.SetAction(MakeOwned<SkipAction>(this));
    // GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
    // ++nextPathNode; // Find alternative paths or if there is none, skip and maybe find a path to other place

    LOG_TRACE("{} {}", name, other.gameobject->name);
}

void Enemy::DebugGUI() {
    int distanceToPlayer {-1};
    
    auto player {scene->FindGameObject(playerRef)};
    if (player) {
        distanceToPlayer = static_cast<int>(glm::distance(player->GetComponent<Transform>().GetPosition(), GetComponent<Transform>().GetPosition())) / TILE_SIZE;
    }

    auto& unitPos {GetComponent<UnitComponent>().GetPosition()};
    ImGui::Begin(name.c_str());
    ImGui::Text("(%i, %i) - %i", unitPos.x, unitPos.y, distanceToPlayer);
    ImGui::End();
}