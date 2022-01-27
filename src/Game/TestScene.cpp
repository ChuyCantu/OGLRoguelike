#include "TestScene.hpp"

#include "Battlers.hpp"
#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Core/Engine.hpp"
#include "Core/GameObject.hpp"
#include "Rendering/Sprite.hpp"
#include "PlayerTest.hpp"
#include "TilemapTest.hpp"
#include "TurnManager.hpp"

#include "UI/Image.hpp"
#include "UI/UIStack.hpp"
#include "UI/Panel.hpp"
#include "Rendering/Sprite.hpp"

TestScene::TestScene(Engine* engine) 
    : Scene{engine} {
    Load();
}

TestScene::~TestScene() {
    TurnManager::Instance().Clear();
}

void TestScene::Load() {
    auto& uiPanel {engine->GetUIStack()->panels[0]};
    auto widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{16.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("default"))));
    widget->SetAnchor(Anchor::TopRight);
    widget->SetRelativePosition(glm::vec2{32.f});     


    AddGameObject<TilemapTest>();

    AddGameObject<BattlerPlayer>();
    AddGameObject<BattlerEnemy>();
}

void TestScene::LastUpdate() {
    TurnManager::Instance().Update();
}