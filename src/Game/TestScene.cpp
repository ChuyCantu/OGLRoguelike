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
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "UI/Image.hpp"
#include "UI/Label.hpp"
#include "UI/Panel.hpp"
#include "UI/Text/TextRenderer.hpp"
#include "UI/UIStack.hpp"
#include "Utils/Color.hpp"

TestScene::TestScene(Engine* engine) 
    : Scene{engine} {
    Load();
}

TestScene::~TestScene() {
    TurnManager::Instance().Clear();
}

Widget* testWidget;
Widget* testChildWidget;
Label* testLabel;
#ifdef CLIP_TEST
Widget* testClip;
#endif  // CLIP_TEST

// #define LAYER_TEST
#define NINE_SLICE_TEST
// #define ANCHOR_TEST
// #define CLIP_TEST

void TempSizeChanged() {
    testChildWidget->SetSize(testChildWidget->GetParent()->GetRect().size / 2.0f);
}

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
    widget->SetPosition(glm::vec2{0.f, 0.f});
    widget->SetRenderOrder(11);
    playerImg = widget;

    //+ BLUE
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                  MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetPosition(glm::vec2{8.f, 8.f});

    //+ GREEN
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                  MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 208}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetPosition(glm::vec2{0.f});
    widget->SetRenderOrder(10);

    //+ YELLOW
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                 MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 256}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetPosition(glm::vec2{-8.f, -8.f});

    //+ WHITE
    widget = uiPanel2->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                 MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{192, 112}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetPosition(glm::vec2{8.f, -8.f});
    widget->SetRenderOrder(-100);

    uiPanel->renderOrder = 0;
    uiPanel2->renderOrder = -100;
#endif

#ifdef NINE_SLICE_TEST
    auto uiPanelSlice{engine->GetUIStack()->AddPanel(
                      MakeOwned<Panel>(Rect{glm::vec2{0.0f, 0.0f},
                                       glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};

    // Not sliced
    widget = uiPanelSlice->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{180.f, 100.f}},
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
                                                    //   MakeRef<Sprite>(AssetManager::GetTexture("default"))));
    widget->SetAnchor(Anchor::Center);
    widget->SetPivot(glm::vec2{0.5f, 0.5f});
    widget->SetPosition(glm::vec2{-uiPanelSlice->GetSize().x / 4.0, uiPanelSlice->GetSize().y / 4.0f});

    // Sliced
    widget = uiPanelSlice->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{180.f, 100.f}},
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
    widget->SetPivot(glm::vec2{0.5f, 0.5f});
    widget->SetPosition(glm::vec2{Camera::GetMainCamera().GetVirtualSize().x / 4.0, Camera::GetMainCamera().GetVirtualSize().y / 4.0f});

    // Reference for debug
    // widget = uiPanelSlice->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{128.f, 64.f}},
    //                                                   MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{192, 112}, glm::ivec2{16})));
    // widget->SetAnchor(Anchor::Center);
    // widget->SetRelativePosition(glm::vec2{Camera::GetMainCamera().GetVirtualSize().x / 4.0, Camera::GetMainCamera().GetVirtualSize().y / 4.0f});
    // dynamic_cast<Image*>(widget)->color = glm::vec4{1.0f, 1.0f, 1.0f, 0.25f};

    // widget = uiPanelSlice->AddWidget(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}, MakeRef<Sprite>(AssetManager::GetTexture("default"))));
    // dynamic_cast<Image*>(widget)->color = glm::vec4{0.f, 0.f, .8f, .2f};

    uiPanelSlice->SetRenderOrder(100);

#endif // NINE_SLICE_TEST

#ifdef CLIP_TEST
    auto clipPanel {engine->GetUIStack()->AddPanel(MakeOwned<Panel>(Rect{glm::vec2{0.f}, Camera::GetMainCamera().GetVirtualSize()}))};
    auto clipParentBorder {clipPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, glm::vec2{310.f, 160.f}},
                                         MakeRef<Sprite>(AssetManager::GetTexture("default"))))};
    clipParentBorder->SetAnchor(Anchor::Center);
    clipParentBorder->SetPivot({0.5f, 0.5f});
    clipParentBorder->SetPosition({0.f, 0.f});                                         

    auto clipParent {clipPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, glm::vec2{300.f, 150.f}},
                                         MakeRef<Sprite>(AssetManager::GetTexture("default"))))};
    dynamic_cast<Image*>(clipParent)->color = glm::vec4{1.0f, 0.f, 0.f, 1.f};
    clipParent->SetAnchor(Anchor::Center);
    clipParent->SetPivot(glm::vec2{0.5f, 0.5f});
    clipParent->SetPosition(glm::vec2{0.f});
    clipPanel->SetRenderOrder(1000);
    clipParent->clipChildren = true;
    testClip = clipParent;

    auto clipChild {clipParent->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, glm::vec2{600.f, 400.f}},
                                         MakeRef<Sprite>(AssetManager::GetTexture("default"))))};
    dynamic_cast<Image*>(clipChild)->color = glm::vec4{1.0f, 0.f, 1.f, 1.f};
    clipChild->SetAnchor(Anchor::Center);
    clipChild->SetPivot(glm::vec2{0.5f, 0.5f});
    clipChild->SetPosition(glm::vec2{0.f});
#endif  // CLIP_TEST

#ifdef ANCHOR_TEST
    //+ Red
    widget = uiPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 112}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::TopLeft);
    widget->SetPivot(glm::vec2{0.0f, 0.0f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Yellow
    widget = uiPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 256}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::TopRight);
    widget->SetPivot(glm::vec2{1.0f, 0.0f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Blue
    widget = uiPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{128.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
    testWidget = widget;
    widget->SetAnchor(Anchor::Center);
    widget->SetPivot(glm::vec2{0.5f, 0.5f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Green
    widget = widget->AddChild/*uiPanel->AddChild*/(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 208}, glm::ivec2{16})));
    testChildWidget = widget;
    // widget->SetAnchor(Anchor::BottomLeft);
    // widget->SetPivot(glm::vec2{0.0f, 1.0f});
    // widget->SetPosition(glm::vec2{0.f});
    widget->GetParent()->onSizeChanged.Subscribe("onSizeChanged", &TempSizeChanged);

    //+ White
    widget = uiPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{192, 112}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::BottomRight);
    widget->SetPivot(glm::vec2{1.0f, 1.0f});
    widget->SetPosition(glm::vec2{0.f});

    uiPanel->SetRenderOrder(99);
#endif    

    AddGameObject<TilemapTest>();

    AddGameObject<BattlerPlayer>();
    AddGameObject<BattlerEnemy>();

    auto go{AddGameObject<GameObject>()};
    auto& sr{go->AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 0}, glm::ivec2{16, 16}), ColorNames::white, 10)};
    go->GetComponent<Transform>().SetPosition(glm::vec2{-16.f, 16.f});

    //+ Font Rendering Tests:
    TextRenderer::LoadFont("resources/assets/fonts/SourceCodePro-Regular.ttf", "SourceCode", 22, FontRenderMode::Raster);
    TextRenderer::LoadFont("resources/assets/fonts/Kenney Pixel Square.ttf", "KenneyPixel");

    auto labelPanel {engine->GetUIStack()->AddPanel(MakeOwned<Panel>(Rect{glm::vec2{0.f}, Camera::GetMainCamera().GetVirtualSize()}))};
    auto labelW{labelPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, {270.f, 66.f}},
                                                      MakeRef<Sprite>(AssetManager::GetTexture("default"))))};
    labelW->SetAnchor(Anchor::Center);
    labelW->SetPivot({0.0f, 0.0f});
    labelW->SetPosition({0.f, 0.f});

    auto labelTest {labelPanel->AddChild(MakeOwned<Label>("Abcde fgh\tijklm\nopqrstuvwxyz"))};
    labelTest->SetSize({200.f, 66.f});  // {269.f, 66.f}
    labelW->SetSize(labelTest->GetSize());
    labelTest->SetAnchor(Anchor::Center);
    labelTest->SetPivot({0.0f, 0.0f});
    labelTest->SetPosition({0.f, 0.f});
    testLabel = dynamic_cast<Label*>(labelTest);
    testLabel->appearance.color = ColorNames::blue;
}

void TestScene::LastUpdate() {
    TurnManager::Instance().Update();

#ifdef CLIP_TEST
    if (testClip) {
        if (Input::GetKeyDown(SDL_SCANCODE_O)) {
            testClip->clipChildren = !testClip->clipChildren;
        }
    }
#endif  // CLIP_TEST

    if (testWidget) {
        if (Input::GetKeyDown(SDL_SCANCODE_KP_4)) {
            testWidget->SetPosition(testWidget->GetPosition() + glm::vec2{-16.f, 0.f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_6)) {
            testWidget->SetPosition(testWidget->GetPosition() + glm::vec2{16.f, 0.f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_2)) {
            testWidget->SetPosition(testWidget->GetPosition() + glm::vec2{0.f, -16.f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_8)) {
            testWidget->SetPosition(testWidget->GetPosition() + glm::vec2{0.f, 16.f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_7)) {
            testWidget->SetSize(glm::vec2{128.f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_9)) {
            testWidget->SetSize(glm::vec2{256.f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_1)) {
            testWidget->SetVisible(true);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_3)) {
            testWidget->SetVisible(false);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_5)) {
            testWidget->GetParent()->RemoveChild(testWidget);
            testWidget = nullptr;
        }
    }

    if (testLabel) {
        if (Input::GetKeyDown(SDL_SCANCODE_K)) {
            testLabel->clipText = !testLabel->clipText;
        }
    }
}