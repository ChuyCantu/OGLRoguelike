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

Widget* playerImg;
#define LAYER_TEST
#define NINE_SLICE_TEST

void TestScene::Load() {
    // auto& uiPanel{engine->GetUIStack()->panels.emplace_back(
    //     MakeOwned<Panel>(Rect{glm::vec2{0.0f, 0.0f}, 
    //                      glm::vec2{Camera::GetMainCamera().GetVirtualSize()} }))};
    auto uiPanel{engine->GetUIStack()->AddPanel(
        MakeOwned<Panel>(Rect{glm::vec2{0.0f, 0.0f}, 
                         glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};
    auto uiPanel2{engine->GetUIStack()->AddPanel(
        MakeOwned<Panel>(Rect{glm::vec2{0.0f, 128.0f}, 
                         glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};                         

    Widget* widget;
#ifdef LAYER_TEST
    //+ RED
    widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                      MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 112}, glm::ivec2{16, 16})));
    widget->SetAnchor(Anchor::TopLeft);
    widget->SetPivot(glm::vec2{0.f, 0.f});
    widget->SetRelativePosition(glm::vec2{0.f, 0.f});
    widget->SetRenderOrder(11);
    playerImg = widget;

    //+ BLUE
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                  MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{8.f, 8.f});

    //+ GREEN
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                  MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 208}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{0.f});
    widget->SetRenderOrder(10);

    //+ YELLOW
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                 MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 256}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{-8.f, -8.f});

    //+ WHITE
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                 MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{192, 112}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{8.f, -8.f});
    widget->SetRenderOrder(-100);

    uiPanel->renderOrder = 0;
    uiPanel2->renderOrder = -100;
#endif

#ifdef NINE_SLICE_TEST
    auto uiPanelSlice{engine->GetUIStack()->AddPanel(
                      MakeOwned<Panel>(Rect{glm::vec2{0.0f, 0.0f},
                                       glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};

    // Not sliced
    widget = uiPanelSlice->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{180.f, 128.f}},
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
                                                    //   MakeRef<Sprite>(AssetManager::GetTexture("default"))));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{-Camera::GetMainCamera().GetVirtualSize().x / 4.0, Camera::GetMainCamera().GetVirtualSize().y / 4.0f});

    // Sliced
    widget = uiPanelSlice->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{180.f, 128.f}},
                             std::vector<Ref<Sprite>>{MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{16, 160}, glm::ivec2{6}), 
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{22, 160}, glm::ivec2{36, 6}),
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{58, 160}, glm::ivec2{6}), 

                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{16, 166}, glm::ivec2{6, 36}),
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{22, 166}, glm::ivec2{36}),
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{58, 166}, glm::ivec2{6, 36}),

                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{16, 202}, glm::ivec2{6}),
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{22, 202}, glm::ivec2{36, 6}),
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{58, 202}, glm::ivec2{6})}));
    widget->SetAnchor(Anchor::Center);
    widget->SetRelativePosition(glm::vec2{Camera::GetMainCamera().GetVirtualSize().x / 4.0, Camera::GetMainCamera().GetVirtualSize().y / 4.0f});

    // Reference for debug
    // widget = uiPanelSlice->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{128.f, 64.f}},
    //                                                   MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{192, 112}, glm::ivec2{16})));
    // widget->SetAnchor(Anchor::Center);
    // widget->SetRelativePosition(glm::vec2{Camera::GetMainCamera().GetVirtualSize().x / 4.0, Camera::GetMainCamera().GetVirtualSize().y / 4.0f});
    // dynamic_cast<Image*>(widget)->color = glm::vec4{1.0f, 1.0f, 1.0f, 0.25f};

    uiPanelSlice->renderOrder = 100;

#endif // NINE_SLICE_TEST

    //! Current Sprite and UI rendererers can render ~1500 elements with fps between 55 and 60 on a RTX 2070 Super
    //! If this becomes a bottleneck, Sprite batching will be implemented, also, each panel will be rendered to a framebuffer and only redraw when needed
    // for (int y = 0; y < 10; ++y) {
    //     for (int x = 0; x < 20; ++x) {
    //         widget = uiPanel->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
    //                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
    //         widget->SetPivot(glm::vec2{0.f, 0.f});
    //         widget->SetRelativePosition(glm::vec2{(float)x * 32.0f, (float)y * 32.0f});
    //         widget->SetRenderOrder(100);
    //     }
    // }

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

    //! Sprite Batching Performance Test
    // for (int x{0}; x < 10; ++x) {
    //     for (int y{0}; y < 10; ++y) {
    //         auto go {AddGameObject<GameObject>()};
    //         auto& sr{go->AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 0}, glm::ivec2{16, 16}), glm::vec4{1.0f}, 10)};
    //         go->GetComponent<Transform>().SetPosition(glm::vec2{x * 16.f - 20.f * 16.f, y * 16.f - 11.f * 16.f});
    //     }
    // }
}

void TestScene::LastUpdate() {
    TurnManager::Instance().Update();

    if (Input::GetKeyDown(SDL_SCANCODE_KP_4)) {
        if (playerImg) {
            dynamic_cast<Image*>(playerImg)->sprite->flipX = true;
        }
    }
    if (Input::GetKeyDown(SDL_SCANCODE_KP_6)) {
        if (playerImg) {
            dynamic_cast<Image*>(playerImg)->sprite->flipX = false;
        }
    }
    if (Input::GetKeyDown(SDL_SCANCODE_KP_2)) {
        if (playerImg) {
            dynamic_cast<Image*>(playerImg)->sprite->flipY = true;
        }
    }
    if (Input::GetKeyDown(SDL_SCANCODE_KP_8)) {
        if (playerImg) {
            dynamic_cast<Image*>(playerImg)->sprite->flipY = false;
        }
    }

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