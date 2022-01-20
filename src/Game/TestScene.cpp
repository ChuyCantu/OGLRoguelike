#include "TestScene.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Core/Engine.hpp"
#include "Core/GameObject.hpp"
#include "Rendering/Sprite.hpp"
#include "PlayerTest.hpp"
#include "TilemapTest.hpp"

TestScene::TestScene(Engine* engine) 
    : Scene{engine} {
    Load();
}

void TestScene::Load() {
    auto player {AddGameObject(MakeOwned<GameObject>(this, "Swordman"))};
    auto& sr {player->AddCommponent<SpriteRenderer>()};
    sr.sprite = MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{48, 112}, glm::ivec2{16, 16});
    player->GetComponent<Transform>().SetPosition(glm::vec3{32.f, 32.f, 0.0f});
    // sr.color = glm::vec4{.0f, 0.f, 1.0f, 1.0f};
    auto& animator {player->AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});
    player->AddCommponent<Collider>().isSolid = false;
    player->tag = "Swordman";
    player->AddCommponent<MoveComponent>().Teleport(player->GetComponent<Transform>().GetPosition());

    auto player2 {AddGameObject(MakeOwned<GameObject>(this, "Archer"))};
    auto& sr2 {player2->AddCommponent<SpriteRenderer>()};
    sr2.sprite = MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{16, 112}, glm::ivec2{16, 16});
    player2->GetComponent<Transform>().SetPosition(glm::vec3{32.f, -64.f, 0.0f});
    // sr2.color = glm::vec4{1.0f, 0.f, 1.0f, 1.0f};
    auto& animator2 {player2->AddCommponent<Animator>()};
    animator2.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator2.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});
    player2->AddCommponent<Collider>();
    player2->tag = "Archer";
    player2->AddCommponent<MoveComponent>().Teleport(player2->GetComponent<Transform>().GetPosition());

    // AddGameObject(MakeOwned<PlayerTest>(this));
    // AddGameObject(MakeOwned<PlayerTest>(this));
    // AddGameObject<PlayerTest>(5);
    AddGameObject<PlayerTest>();

    // AddGameObject(MakeOwned<Tilemap>(this));
    AddGameObject<TilemapTest>();

    // auto tilemap2 {AddGameObject<Tilemap>()};
    // tilemap2->GetComponent<Transform>().SetPosition(glm::vec3{0.f, -170.f, 0.f});

}