#include "Enemy.hpp"

#include "Game/Action.hpp"
#include "Game/Constants.hpp"
#include "Game/DungeonGen/Dungeon.hpp"
#include "Input/Input.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Core/Engine.hpp"
#include "Rendering/Renderer.hpp"

#include <imgui.h>

Enemy::Enemy(Scene* scene, const std::string& name) : Unit{scene, name} {
    tag = "Enemy";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("slime0_spritesheet"), glm::ivec2{16, 16}, glm::ivec2{TILE_SIZE, TILE_SIZE}), ColorNames::white, 0)};

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("slime0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("slime1_spritesheet"), 0.5f});

    AddCommponent<Collider>();
    // AddCommponent<MoveComponent>().Teleport(glm::vec3{0.f, 0.f, 0.0f});
    AddCommponent<UnitComponent>(1, 10, 0, 100);
}

void Enemy::Update() {
    glm::ivec2 playerPos;
    auto player{scene->FindGameObject(playerRef)};
    if (player) {
        auto& playerTransform{player->GetComponent<Transform>()};
        playerPos = glm::ivec2{playerTransform.GetPosition().x, playerTransform.GetPosition().y};
    }

    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        // GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
        if (currentPath.empty() || nextPathNode == 0) {
            auto& transform{GetComponent<Transform>()};
            glm::ivec2 pos{transform.GetPosition().x, transform.GetPosition().y};
            glm::ivec2 pathStart{pos / TILE_SIZE};
            if (pos.x < 0) --pathStart.x;
            if (pos.y < 0) --pathStart.y;

            glm::ivec2 pathEnd{playerPos / TILE_SIZE};
            if (playerPos.x < 0) --pathEnd.x;
            if (playerPos.y < 0) --pathEnd.y;

            if (!dungeon->pathfinding.FindPath(pathStart, pathEnd, currentPath, DiagonalMovement::OnlyWhenNoObstacles)) {
                currentPath.clear();
                GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
            }
            nextPathNode = currentPath.size() - 1;
        }

        GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, currentPath[--nextPathNode] * TILE_SIZE, 0.15f, dungeon));
    }

    if (!currentPath.empty()) {
        for (size_t i{0}; i < currentPath.size() - 1; ++i) {
            glm::ivec2 p{currentPath[i + 1] - currentPath[i]};
            if (p.x != 0 && p.y != 0) {  // diagonal move
                scene->GetEngine()->GetRenderer()->DrawLine2D(currentPath[i] * TILE_SIZE + 8, currentPath[i + 1] * TILE_SIZE + 8, Color2Vec3(ColorNames::white));

                // adjacent nodes
                scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{currentPath[i].x, currentPath[i + 1].y} * TILE_SIZEF, glm::vec2{currentPath[i].x, currentPath[i + 1].y} * TILE_SIZEF + TILE_SIZEF, Color2Vec3(ColorNames::cyan));
                scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{currentPath[i].x, currentPath[i + 1].y} * TILE_SIZEF + glm::vec2{0.f, TILE_SIZEF}, glm::vec2{currentPath[i].x, currentPath[i + 1].y} * TILE_SIZEF + glm::vec2{TILE_SIZEF, 0.f}, Color2Vec3(ColorNames::cyan));
                scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{currentPath[i + 1].x, currentPath[i].y} * TILE_SIZEF, glm::vec2{currentPath[i + 1].x, currentPath[i].y} * TILE_SIZEF + TILE_SIZEF, Color2Vec3(ColorNames::cyan));
                scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{currentPath[i + 1].x, currentPath[i].y} * TILE_SIZEF + glm::vec2{0.f, TILE_SIZEF}, glm::vec2{currentPath[i + 1].x, currentPath[i].y} * TILE_SIZEF + glm::vec2{TILE_SIZEF, 0.f}, Color2Vec3(ColorNames::cyan));
            } else
                scene->GetEngine()->GetRenderer()->DrawLine2D(currentPath[i] * 16 + 8, currentPath[i + 1] * 16 + 8, Color2Vec3(ColorNames::white));
        }
    }
}

void Enemy::OnCollisionEnter(const Collider& other) {
    // auto& unitComp {GetComponent<UnitComponent>()};
    // unitComp.GetAction()->Cancel();
    // unitComp.SetAction(MakeOwned<SkipAction>(this));
    GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
    ++nextPathNode; // Find alternative paths or if there is none, skip and maybe find a path to other place
}

void Enemy::DebugGUI() {
    int distanceToPlayer {-1};
    
    auto player {scene->FindGameObject(playerRef)};
    if (player) {
        distanceToPlayer = static_cast<int>(glm::distance(player->GetComponent<Transform>().GetPosition(), GetComponent<Transform>().GetPosition())) / TILE_SIZE;
    }

    ImGui::Begin(name.c_str());
    ImGui::Text("distance: %i", distanceToPlayer);
    ImGui::End();
}