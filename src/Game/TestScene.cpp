#include "TestScene.hpp"

#include "Core/AssetsManager.hpp"
#include "Core/Components.hpp"
#include "Core/Engine.hpp"
#include "Core/GameObject.hpp"
#include "Rendering/Sprite.hpp"
#include "PlayerTest.hpp"

TestScene::TestScene(Engine* engine) 
    : Scene{engine} {
    Load();
}

void TestScene::Load() {
    auto player {AddGameObject(MakeOwned<GameObject>(this, "Swordman"))};
    auto& sr {player->AddCommponent<SpriteRenderer>()};
    sr.sprite = MakeRef<Sprite>(AssetsManager::GetTexture("player0_spritesheet"), glm::ivec2{48, 112}, glm::ivec2{16, 16});
    player->GetComponent<Transform>().SetPosition(glm::vec3{32.f, 32.f, 0.0f});
    sr.color = glm::vec4{.0f, 0.f, 1.0f, 1.0f};

    auto player2 {AddGameObject(MakeOwned<GameObject>(this, "Archer"))};
    auto& sr2 {player2->AddCommponent<SpriteRenderer>()};
    sr2.sprite = MakeRef<Sprite>(AssetsManager::GetTexture("player0_spritesheet"), glm::ivec2{16, 112}, glm::ivec2{16, 16});
    player2->GetComponent<Transform>().SetPosition(glm::vec3{32.f, -64.f, 0.0f});
    sr2.color = glm::vec4{1.0f, 0.f, 1.0f, 1.0f};

    AddGameObject(MakeOwned<PlayerTest>(this));
}