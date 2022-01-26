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

#include "UI/Panel.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Camera.hpp"

TestScene::TestScene(Engine* engine) 
    : Scene{engine} {
    Load();
}

TestScene::~TestScene() {
    TurnManager::Instance().Clear();
}

void TestScene::Load() {
    uiPanel = MakeOwned<Panel>();
    auto& virtualScreen {Camera::GetMainCamera().GetVirtualSize()};
    uiPanel->rect = Rect{glm::vec2{0.0f, 0.0f}, glm::vec2{virtualScreen}};
    uiPanel->widgets.emplace_back(MakeOwned<Widget>(uiPanel.get(), Rect{glm::vec2{0.f}, glm::vec2{16.f}}));

    uiPanel->widgets[0]->SetAnchor(Anchor::BottomRight);
    uiPanel->widgets[0]->SetPivot(glm::vec2{1.f, 1.f});
    uiPanel->widgets[0]->SetRelativePosition(glm::vec2{16.f, 0.f});

    AddGameObject<TilemapTest>();

    AddGameObject<BattlerPlayer>();
    AddGameObject<BattlerEnemy>();

}

void TestScene::LastUpdate() {
    TurnManager::Instance().Update();
}