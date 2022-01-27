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

#define LAYER_TEST
void TestScene::Load() {
    // auto& uiPanel{engine->GetUIStack()->panels.emplace_back(
    //     MakeOwned<Panel>(Rect{glm::vec2{0.0f, 0.0f}, 
    //                      glm::vec2{Camera::GetMainCamera().GetVirtualSize()} }))};
    auto uiPanel{engine->GetUIStack()->AddPanel(
        MakeOwned<Panel>(Rect{glm::vec2{100.0f, 0.0f}, 
                         glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};
    auto uiPanel2{engine->GetUIStack()->AddPanel(
        MakeOwned<Panel>(Rect{glm::vec2{200.0f, 0.0f}, 
                         glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};                         

#ifdef LAYER_TEST
    //+ YELLOW
    auto widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 32}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{-8.f, 8.f});
    widget->SetRenderOrder(11);

    //+ GREEN
    widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                 MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 80}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{8.f, 8.f});

    //+ BLUE
    widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                 MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 128}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{0.f});
    widget->SetRenderOrder(10);

    //+ RED
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                 MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 176}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{-8.f, -8.f});

    //+ BLACK
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                 MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{128, 32}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{8.f, -8.f});
    widget->SetRenderOrder(-100);

    uiPanel->renderOrder = 0;
    uiPanel2->renderOrder = -100;

#endif

#ifdef ANCHOR_TEST
    auto widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 32}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetPivot(glm::vec2{0.5f, 0.5f});
    // widget->SetRelativePosition(glm::vec2{16.f, 16.f});
    widget->SetRelativePosition(glm::vec2{0.f});

    widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 80}, glm::ivec2{16})));
    // widget->SetRelativePosition(glm::vec2{16.f, 16.f});
    widget->SetRelativePosition(glm::vec2{0.f});

    widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 128}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::BottomRight);
    // widget->SetRelativePosition(glm::vec2{16.f, 16.f});
    widget->SetRelativePosition(glm::vec2{0.f});

    widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 176}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::TopRight);
    // widget->SetRelativePosition(glm::vec2{32.f});
    widget->SetRelativePosition(glm::vec2{0.f});
    
    widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{128, 32}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::BottomLeft);
    // widget->SetRelativePosition(glm::vec2{32.f});
    widget->SetRelativePosition(glm::vec2{0.f});
#endif    

    AddGameObject<TilemapTest>();

    AddGameObject<BattlerPlayer>();
    AddGameObject<BattlerEnemy>();
}

void TestScene::LastUpdate() {
    TurnManager::Instance().Update();

    // if (Input::GetKeyDown(SDL_SCANCODE_D)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->SetPosition(uiPanel->GetRect().position + glm::vec2{16.0f, 0.f});
    //     // uiPanel->SetRect(Rect{uiPanel->GetRect().position + glm::vec2{16.0f, 0.f}, uiPanel->GetRect().size});
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_A)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->SetPosition(uiPanel->GetRect().position + glm::vec2{-16.0f, 0.f});
    //     // uiPanel->SetRect(Rect{uiPanel->GetRect().position + glm::vec2{-16.0f, 0.f}, uiPanel->GetRect().size});
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_W)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->SetPosition(uiPanel->GetRect().position + glm::vec2{0.0f, -16.f});
    //     // uiPanel->SetRect(Rect{uiPanel->GetRect().position + glm::vec2{.0f, -16.f}, uiPanel->GetRect().size});
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_S)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->SetPosition(uiPanel->GetRect().position + glm::vec2{0.0f, 16.f});
    //     // uiPanel->SetRect(Rect{uiPanel->GetRect().position + glm::vec2{.0f, 16.f}, uiPanel->GetRect().size});
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_F)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->SetVisible(!uiPanel->IsVisible());
    // }

    // if (Input::GetKeyDown(SDL_SCANCODE_K)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->SetSize(glm::vec2{Camera::GetMainCamera().GetVirtualSize()});
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_L)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->SetSize(glm::vec2{Camera::GetMainCamera().GetVirtualSize()} / 2.f);
    // }

    // if (Input::GetKeyDown(SDL_SCANCODE_C)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->widgets[0]->SetAnchor(Anchor::TopLeft);
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_V)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->widgets[0]->SetAnchor(Anchor::TopRight);
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_B)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->widgets[0]->SetAnchor(Anchor::Center);
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_N)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->widgets[0]->SetAnchor(Anchor::BottomLeft);
    // }
    // if (Input::GetKeyDown(SDL_SCANCODE_M)) {
    //     auto& uiPanel{engine->GetUIStack()->panels[0]};
    //     uiPanel->widgets[0]->SetAnchor(Anchor::BottomLeft);
    // }
}