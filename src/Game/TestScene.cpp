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
#include "Input/Input.hpp"

#include "Rendering/Camera.hpp"
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

    if (Input::GetKeyDown(SDL_SCANCODE_D)) {
        auto& uiPanel{engine->GetUIStack()->panels[0]};
        uiPanel->SetPosition(uiPanel->GetRect().position + glm::vec2{16.0f, 0.f});
        // uiPanel->SetRect(Rect{uiPanel->GetRect().position + glm::vec2{16.0f, 0.f}, uiPanel->GetRect().size});
    }
    if (Input::GetKeyDown(SDL_SCANCODE_A)) {
        auto& uiPanel{engine->GetUIStack()->panels[0]};
        uiPanel->SetPosition(uiPanel->GetRect().position + glm::vec2{-16.0f, 0.f});
        // uiPanel->SetRect(Rect{uiPanel->GetRect().position + glm::vec2{-16.0f, 0.f}, uiPanel->GetRect().size});
    }
    if (Input::GetKeyDown(SDL_SCANCODE_W)) {
        auto& uiPanel{engine->GetUIStack()->panels[0]};
        uiPanel->SetPosition(uiPanel->GetRect().position + glm::vec2{0.0f, -16.f});
        // uiPanel->SetRect(Rect{uiPanel->GetRect().position + glm::vec2{.0f, -16.f}, uiPanel->GetRect().size});
    }
    if (Input::GetKeyDown(SDL_SCANCODE_S)) {
        auto& uiPanel{engine->GetUIStack()->panels[0]};
        uiPanel->SetPosition(uiPanel->GetRect().position + glm::vec2{0.0f, 16.f});
        // uiPanel->SetRect(Rect{uiPanel->GetRect().position + glm::vec2{.0f, 16.f}, uiPanel->GetRect().size});
    }
    if (Input::GetKeyDown(SDL_SCANCODE_F)) {
        auto& uiPanel{engine->GetUIStack()->panels[0]};
        uiPanel->SetVisible(!uiPanel->IsVisible());
    }

    if (Input::GetKeyDown(SDL_SCANCODE_K)) {
        auto& uiPanel{engine->GetUIStack()->panels[0]};
        uiPanel->SetSize(glm::vec2{Camera::GetMainCamera().GetVirtualSize()});
    }
    if (Input::GetKeyDown(SDL_SCANCODE_L)) {
        auto& uiPanel{engine->GetUIStack()->panels[0]};
        uiPanel->SetSize(glm::vec2{Camera::GetMainCamera().GetVirtualSize()} / 2.f);
    }
}