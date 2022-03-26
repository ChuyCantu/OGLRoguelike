#include "Enemy.hpp"

#include "Game/Action.hpp"
#include "Game/DungeonGen/Dungeon.hpp"
#include "Input/Input.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Core/Engine.hpp"
#include "Rendering/Renderer.hpp"

#include <imgui.h>

Enemy::Enemy(Scene* scene, const std::string& name) : Unit{scene, name} {
    tag = "Enemy";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("slime0_spritesheet"), glm::ivec2{16, 16}, glm::ivec2{16, 16}), ColorNames::white, 0)};

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("slime0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("slime1_spritesheet"), 0.5f});

    AddCommponent<Collider>();
    AddCommponent<MoveComponent>().Teleport(glm::vec3{0.f, 0.f, 0.0f});
    AddCommponent<UnitComponent>(1, 10, 0, 100);
}

void Enemy::Update() {
    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        // if (Input::GetKeyDown(SDL_SCANCODE_S)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
        // }
    }

    auto& transform{GetComponent<Transform>()};
    glm::ivec2 pos{transform.GetPosition().x, transform.GetPosition().y};
    glm::ivec2 pathStart{pos / 16};
    if (pos.x < 0) --pathStart.x;
    if (pos.y < 0) --pathStart.y;

    glm::ivec2 playerPos;
    auto player{scene->FindGameObject(playerRef)};
    if (player) {
        auto& playerTransform{player->GetComponent<Transform>()};
        playerPos = glm::ivec2{playerTransform.GetPosition().x, playerTransform.GetPosition().y};
    }
    glm::ivec2 pathEnd{playerPos / 16};
    if (playerPos.x < 0) --pathEnd.x;
    if (playerPos.y < 0) --pathEnd.y;

    std::vector<glm::ivec2> path;
    // if (dungeon->pathfinding.FindPath(pathStart, pathEnd, path, true)) {
    if (dungeon->pathfinding.FindPath(pathStart, pathEnd, path, DiagonalMovement::OnlyWhenNoObstacles)) {
        // Paint path
        for (size_t i{0}; i < path.size() - 1; ++i) {
            glm::ivec2 p{path[i + 1] - path[i]};
            if (p.x != 0 && p.y != 0) { // diagonal move
                scene->GetEngine()->GetRenderer()->DrawLine2D(path[i] * 16 + 8, path[i + 1] * 16 + 8, Color2Vec3(ColorNames::white));

                // adjacent nodes
                scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{path[i].x, path[i + 1].y} * 16.0f, glm::vec2{path[i].x, path[i + 1].y} * 16.0f + 16.0f, Color2Vec3(ColorNames::cyan));
                scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{path[i].x, path[i + 1].y} * 16.0f + glm::vec2{0.f, 16.0f}, glm::vec2{path[i].x, path[i + 1].y} * 16.0f + glm::vec2{16.0f, 0.f}, Color2Vec3(ColorNames::cyan));
                scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{path[i + 1].x, path[i].y} * 16.0f, glm::vec2{path[i + 1].x, path[i].y} * 16.0f + 16.0f, Color2Vec3(ColorNames::cyan));
                scene->GetEngine()->GetRenderer()->DrawLine2D(glm::vec2{path[i + 1].x, path[i].y} * 16.0f + glm::vec2{0.f, 16.0f}, glm::vec2{path[i + 1].x, path[i].y} * 16.0f + glm::vec2{16.0f, 0.f}, Color2Vec3(ColorNames::cyan));
            }
            else
                scene->GetEngine()->GetRenderer()->DrawLine2D(path[i] * 16 + 8, path[i + 1] * 16 + 8, Color2Vec3(ColorNames::white));
        }
    }
}

void Enemy::DebugGUI() {
    int distanceToPlayer {-1};
    
    auto player {scene->FindGameObject(playerRef)};
    if (player) {
        distanceToPlayer = static_cast<int>(glm::distance(player->GetComponent<Transform>().GetPosition(), GetComponent<Transform>().GetPosition())) / 16;
    }

    ImGui::Begin(name.c_str());
    ImGui::Text("distance: %i", distanceToPlayer);
    ImGui::End();
}