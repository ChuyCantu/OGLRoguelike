#include "TestScene.hpp"

#include "Battlers.hpp"
#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Core/Engine.hpp"
#include "Core/GameObject.hpp"
#include "Rendering/Sprite.hpp"
#include "PlayerTest.hpp"
#include "Game/TurnManager.hpp"
#include "Input/Input.hpp"
#include "Utils/Random.hpp"
#include "Rendering/Renderer.hpp"

#include "Rendering/Camera.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "UI/Image.hpp"
#include "UI/Label.hpp"
#include "UI/Panel.hpp"
#include "UI/Text/TextRenderer.hpp"
#include "UI/UIStack.hpp"
#include "UI/UI.hpp"
#include "Utils/Color.hpp"
#include "UI/Button.hpp"
#include "UI/Checkbox.hpp"
#include "UI/Slider.hpp"
#include "UI/ScrollView.hpp"
#include "UI/UI.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

TestScene::TestScene(Engine* engine) 
    : Scene{engine} {
    Load();
}

TestScene::~TestScene() {
    TurnManager::Instance().Clear();
}

Widget* testWidget;
Widget* testChildWidget;
Label* testLabel {nullptr};
Widget* testButton {nullptr};
Checkbox* testCheckbox {nullptr};
Slider* testSlider {nullptr};
Slider* testSlider2 {nullptr};
Scrollbar* testScrollbar {nullptr};
ScrollView* scrollviewTest {nullptr};
entt::entity testPlayer;
entt::entity testGO;

#ifdef CLIP_TEST
    Widget* testClip;
#endif  // CLIP_TEST

// #define LAYER_TEST
// #define NINE_SLICE_TEST
#define ANCHOR_TEST
Widget* anchorPanelTest {nullptr};
// #define CLIP_TEST

void TempSizeChanged() {
    testChildWidget->SetSize(testChildWidget->GetParent()->GetRect().size / 2.0f);
}

void TestScene::Load() {
    auto player {AddGameObject<UnitPlayer>()};
    player->GetComponent<Transform>().SetPosition({-32.f, 16.f});
    AddGameObject<UnitEnemy>();
    testPlayer = player->Entity();

    auto go{AddGameObject<GameObject>()};
    auto& sr{go->AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 0}, glm::ivec2{16, 16}), ColorNames::white, 10)};
    testGO = go->Entity();
    go->GetComponent<Transform>().SetPosition(glm::vec2{-64.f, 16.f});

    player->GetComponent<Transform>().AddChild(go);

    //+ Font Rendering Tests:
    TextRenderer::LoadFont("resources/assets/fonts/SourceCodePro-Regular.ttf", "SourceCode", 22, FontRenderMode::Raster);
    // TextRenderer::LoadFont("resources/assets/fonts/SourceCodePro-Regular.ttf", "SourceCode");
    TextRenderer::LoadFont("resources/assets/fonts/Kenney Pixel Square.ttf", "KenneyPixel");
    TextRenderer::LoadFont("resources/assets/fonts/SHPinscher-Regular.otf", "SHPinscher");
    // TextRenderer::LoadFont("resources/assets/fonts/Silver.ttf", "Silver");

    // auto& uiPanel{engine->GetUIStack()->panels.emplace_back(
    //     MakeOwned<Panel>(Rect{glm::vec2{0.0f, 0.0f}, 
    //                      glm::vec2{Camera::GetMainCamera().GetVirtualSize()} }))};
    auto uiPanel{engine->GetUIStack()->AddChild(
        MakeOwned<Panel>(Rect{glm::vec2{0.0f, 0.0f},
                              glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};
    // auto uiPanel2{engine->GetUIStack()->AddPanel(
    //     MakeOwned<Panel>(Rect{glm::vec2{0.0f, 128.0f}, 
    //                      glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};                         

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
    auto uiPanelSlice{engine->GetUIStack()->AddChild(
                      MakeOwned<Panel>(Rect{glm::vec2{0.0f, 0.0f},
                                       glm::vec2{Camera::GetMainCamera().GetVirtualSize()}}))};

    // Not sliced
    widget = uiPanelSlice->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{180.f, 100.f}},
                                                      MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
                                                    //   MakeRef<Sprite>(AssetManager::GetTexture("default"))));
    widget->SetAnchor(Anchor::Center);
    widget->SetPivot(glm::vec2{0.5f, 0.5f});
    // widget->SetPosition(glm::vec2{0.0f});
    widget->SetPosition(glm::vec2{-uiPanelSlice->GetSize().x / 4.0, uiPanelSlice->GetSize().y / 4.0f});
    widget->SetRenderOrder(10);

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
    widget->SetRenderOrder(9);

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
    auto anchorPanel {UI::AddPanel(MakeOwned<Panel>())};
    anchorPanelTest = anchorPanel;

    //+ Red
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 112}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::TopLeft);
    widget->SetPivot(glm::vec2{0.0f, 0.0f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Green
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 208}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Top);
    widget->SetPivot(glm::vec2{0.5f, 0.0f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Yellow
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 256}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::TopRight);
    widget->SetPivot(glm::vec2{1.0f, 0.0f});
    widget->SetPosition(glm::vec2{0.f});

    //+ White
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{192, 112}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Left);
    widget->SetPivot(glm::vec2{0.0f, 0.5f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Blue
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Center);
    widget->SetPivot(glm::vec2{0.5f, 0.5f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Red
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 112}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Right);
    widget->SetPivot(glm::vec2{1.0f, 0.5f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Green
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 208}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::BottomLeft);
    widget->SetPivot(glm::vec2{0.0f, 1.0f});
    widget->SetPosition(glm::vec2{0.f});

    //+ Yellow
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 256}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::Bottom);
    widget->SetPivot(glm::vec2{0.5f, 1.0f});
    widget->SetPosition(glm::vec2{0.f});

    //+ White
    widget = anchorPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}}, 
                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{192, 112}, glm::ivec2{16})));
    widget->SetAnchor(Anchor::BottomRight);
    widget->SetPivot(glm::vec2{1.0f, 1.0f});
    widget->SetPosition(glm::vec2{0.f});

    uiPanel->SetRenderOrder(101);
#endif    

    auto labelPanel {engine->GetUIStack()->AddChild(MakeOwned<Panel>(Rect{glm::vec2{0.f}, Camera::GetMainCamera().GetVirtualSize()}))};
    labelPanel->SetRenderOrder(-1000);
    auto labelW{labelPanel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, {270.f, 90.f}},
                                                      MakeRef<Sprite>(AssetManager::GetTexture("default"))))};
    labelW->SetAnchor(Anchor::Center);
    labelW->SetPivot({0.0f, 0.0f});
    labelW->SetPosition({0.f, 0.f});
    labelW->SetRenderOrder(5);

    auto labelTest {labelPanel->AddChild(MakeOwned<Label>("Abcde fgh\tijklm\nopqrstuvwxyz", 22,  glm::vec2{250.f, 150.f}/*, glm::vec2{200.f, 66.f}*/))};
    labelW->SetSize(labelTest->GetSize());
    labelTest->SetAnchor(Anchor::Center);
    labelTest->SetPivot({0.0f, 0.0f});
    labelTest->SetPosition({0.f, 0.f});
    testLabel = dynamic_cast<Label*>(labelTest);
    testLabel->appearance.color = ColorNames::blue;
    testLabel->horizontalAlign = TextHorzAlign::Right;
    // testLabel->font.name = "SourceCode";
    // testLabel->font.size = 22;
    // testLabel->font.mode = FontRenderMode::Raster;
    testLabel->SetFont(Font{"SourceCode", 22, FontRenderMode::Raster});
    labelTest->SetRenderOrder(50);
    // labelPanel->SetVisible(false);

    labelTest->onClick.Subscribe("onClick", 
        [](Widget* source, EventHandler& eventHandler) {
            LOG_TRACE("Clicked!");
            // eventHandler.handled = false;
        });
    labelTest->onMouseEnter.Subscribe("onMouseEnter", 
        [](Widget* source, EventHandler& eventHandler) {
            // eventHandler.handled = false;
            LOG_TRACE("Mouse Entered!");
        });
    labelTest->onMouseExit.Subscribe("onMouseExit", 
        [](Widget* source, EventHandler& eventHandler) {
            // eventHandler.handled = false;
            LOG_TRACE("Mouse Exited!");
        });
    labelTest->ignoreInput = false;
    // labelPanel->childrenIgnoreInput = true;

    auto button {uiPanel->AddChild(MakeOwned<Button>("Button", glm::vec2{100.f, 30.f}))};
    button->SetPosition(glm::vec2{100.f, 320.f});
    uiPanel->SetRenderOrder(1000);
    // button->SetEnabled(false);
    testButton = button;
    testButton->onClick.Subscribe("click", 
        [](Widget* source, EventHandler& event) {
            LOG_TRACE("Button clicked");
        });
    auto b {dynamic_cast<Button*>(testButton)};
    b->label->SetFont(Font{"SHPinscher", 64, FontRenderMode::SDF});
    b->label->SetTextSize(30);
    b->label->appearance.borderWidth = 0;
    // b->label->verticalAlign = TextVertAlign::Bottom;

    // dynamic_cast<UnitPlayer*>(player)->testLabel = testLabel;

    auto checkbox {uiPanel->AddChild(MakeOwned<Checkbox>(Rect{{100.f, 200.f}, {16.f, 16.f}}))};
    testCheckbox = dynamic_cast<Checkbox*>(checkbox);

    auto slider {uiPanel->AddChild(MakeOwned<Slider>())};
    slider->SetPosition({100.f, 230.f});
    testSlider = dynamic_cast<Slider*>(slider);
    testSlider->SetRange(0, 100);
    // testSlider->onValueChanged.Subscribe("SliderOnV", [](Widget*, float v) { LOG_TRACE("Value changed: {}", v)});
    // testSlider->SetSize({300.f, 10.f});

    slider = uiPanel->AddChild(MakeOwned<Slider>());
    testSlider2 = dynamic_cast<Slider*>(slider);
    // testSlider2->SetOrientation(Orientation::Vertical);
    testSlider2->SetDirection(SliderDirection::BottomToTop);
    testSlider2->SetRange(-5, 5);
    slider->SetPosition({220.f, 230.f});

    // dynamic_cast<UnitPlayer*>(player)->sliderTest = testSlider;
    // dynamic_cast<UnitPlayer*>(player)->sliderTest2 = testSlider2;

    auto scrollbar {uiPanel->AddChild(MakeOwned<Scrollbar>())};
    scrollbar->SetPosition({80.f, 250.f});
    dynamic_cast<Scrollbar*>(scrollbar)->SetThumbSize(0.2f);

    auto scrollbar2 {uiPanel->AddChild(MakeOwned<Scrollbar>())};
    scrollbar2->SetPosition({100.f, 250.f});
    dynamic_cast<Scrollbar*>(scrollbar2)->SetOrientation(Orientation::Horizontal);
    testScrollbar = dynamic_cast<Scrollbar*>(scrollbar2);
    // testScrollbar->SetSize({150.f, 10.f});
    // testScrollbar->SetSize({10.f, 150.f});

    auto scrollview {uiPanel->AddChild(MakeOwned<ScrollView>("TestScrollView"))};
    scrollview->SetPosition({120.f, 40.f});
    // scrollviewTest = dynamic_cast<ScrollView*>(scrollview);
    scrollviewTest = dynamic_cast<ScrollView*>(UI::Stack()->FindChild("TestScrollView", true));
    glm::vec2 testContentSize {90.f, 140.f};
    // glm::vec2 testContentSize{50.f, 50.f};
    scrollviewTest->Content()->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, testContentSize},
                                                         MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16})));
    scrollviewTest->Content()->SetSize(testContentSize);  // This should be automatically when using Layout panels (planned for future)
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

    if (Input::GetKeyDown(SDL_SCANCODE_M)) {
        if (UI::hovered)
            LOG_TRACE("hovered: order: {}. parent: {}.", UI::hovered->GetRenderOrder(), UI::hovered->GetParent()->GetRenderOrder());
        if (UI::focused)
            LOG_TRACE("focused: order: {}. parent: {}.", UI::focused->GetRenderOrder(), UI::focused->GetParent()->GetRenderOrder());
    }

    if (testButton && Input::GetKeyDown(SDL_SCANCODE_N)) {
        testButton->SetEnabled(!testButton->IsEnabled());
        testButton->SetSize(glm::vec2{200.f, 25.f});
        testButton->SetPivot(glm::vec2{0.5f, 0.5f});
        testButton->SetPosition(testButton->GetPosition());
    }

    if (testCheckbox && Input::GetKeyDown(SDL_SCANCODE_B)) {
        LOG_TRACE("Checkbox checked: {}", testCheckbox->checked);
        testCheckbox->SetSize(glm::vec2{30.f, 30.f});
    }

    if (testSlider && testSlider2 && Input::GetKeyDown(SDL_SCANCODE_V)) {
        switch (testSlider->GetDirection()) {
            case SliderDirection::LeftToRight:
                testSlider->SetDirection(SliderDirection::RightToLeft);
                break;
            case SliderDirection::RightToLeft:
                testSlider->SetDirection(SliderDirection::LeftToRight);
                break;
            case SliderDirection::BottomToTop:
                testSlider->SetDirection(SliderDirection::TopToBottom);
                break;
            case SliderDirection::TopToBottom:
                testSlider->SetDirection(SliderDirection::BottomToTop);
                break;
        }

        switch (testSlider2->GetDirection()) {
            case SliderDirection::LeftToRight:
                testSlider2->SetDirection(SliderDirection::RightToLeft);
                break;
            case SliderDirection::RightToLeft:
                testSlider2->SetDirection(SliderDirection::LeftToRight);
                break;
            case SliderDirection::BottomToTop:
                testSlider2->SetDirection(SliderDirection::TopToBottom);
                break;
            case SliderDirection::TopToBottom:
                testSlider2->SetDirection(SliderDirection::BottomToTop);
                break;
        }

        // testSlider->InvertDirection();
        // testSlider2->InvertDirection();
    }

    // if (testSlider && testSlider2 && Input::GetKeyDown(SDL_SCANCODE_C)) {
    //     testSlider->SetValue(Random::Range(testSlider->min, testSlider->max));
    // }

#ifdef ANCHOR_TEST
    if (anchorPanelTest) {
        static bool orSize {true};
        if (Input::GetKeyDown(SDL_SCANCODE_5)) {
            if (orSize) 
                anchorPanelTest->SetSize(Camera::GetMainCamera().GetVirtualSize() / 2);
            else
                anchorPanelTest->SetSize(Camera::GetMainCamera().GetVirtualSize());

            orSize = !orSize;
        }
        if (Input::GetKeyDown(SDL_SCANCODE_W)) {
            anchorPanelTest->SetAbsolutePosition(anchorPanelTest->GetAbsolutePivotPosition() + glm::vec2{0.0f, -16.0f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_S)) {
            anchorPanelTest->SetAbsolutePosition(anchorPanelTest->GetAbsolutePivotPosition() + glm::vec2{0.0f, 16.0f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_A)) {
            anchorPanelTest->SetAbsolutePosition(anchorPanelTest->GetAbsolutePivotPosition() + glm::vec2{-16.0f, 0.0f});
        }
        if (Input::GetKeyDown(SDL_SCANCODE_D)) {
            anchorPanelTest->SetAbsolutePosition(anchorPanelTest->GetAbsolutePivotPosition() + glm::vec2{16.0f, 0.0f});
        }
    }
#endif
    if (testSlider && Input::GetKeyDown(SDL_SCANCODE_KP_4)) {
        float v{Random::Range(testSlider->GetMin(), testSlider->GetMax())};
        LOG_TRACE("Rand slider value: {}", v);
        testSlider->SetValueWithoutNotify(v);
    }
    if (testSlider && Input::GetKeyDown(SDL_SCANCODE_KP_5)) {
        float v {Random::Range(testSlider->GetMin(), testSlider->GetMax())};
        LOG_TRACE("Rand slider value: {}", v);
        testSlider->SetValue(v);
    }
    if (testSlider && Input::GetKeyDown(SDL_SCANCODE_KP_6)) {
        static bool whole {false};
        whole = !whole;
        testSlider->UseWholeNumbers(whole);
        LOG_TRACE("Whole?: {}", whole);
    }

    if (testScrollbar && Input::GetKeyDown(SDL_SCANCODE_KP_8)) {
        testScrollbar->SetThumbSize(Random::Range(0.2f, 1.0f));
    }

    if (scrollviewTest) {
        if (Input::GetKeyDown(SDL_SCANCODE_7)) {
            scrollviewTest->Content()->SetSize(scrollviewTest->Content()->GetSize() * 0.5f);
            scrollviewTest->Content()->GetChildren()[0]->SetSize(scrollviewTest->Content()->GetSize());
        }
        if (Input::GetKeyDown(SDL_SCANCODE_8)) {
            scrollviewTest->Content()->SetSize(scrollviewTest->Content()->GetSize() * 2.f);
            scrollviewTest->Content()->GetChildren()[0]->SetSize(scrollviewTest->Content()->GetSize());
        }
    }
}

void TestScene::DebugGUI() {
    if (scrollviewTest) {
        ImGui::Begin("ScrollView");
        ImGui::Text("H: %f", scrollviewTest->horizontalScrollbar->GetValue());
        ImGui::Text("V: %f", scrollviewTest->verticalScrollbar->GetValue());
        ImGui::Text("Content w: %f", scrollviewTest->Content()->GetSize().x);
        ImGui::Text("Content h: %f", scrollviewTest->Content()->GetSize().y);
        ImGui::Text("Content x: %f", scrollviewTest->Content()->GetPosition().x);
        ImGui::Text("Content y: %f", scrollviewTest->Content()->GetPosition().y);
        ImGui::End();
    }

    GameObject* player {FindGameObject(testPlayer)};
    if (player) {
        ImGui::Begin("TestPlayer");
        ImGui::Text("name: %s", player->name.c_str());
        ImGui::End();

        if (Input::GetKeyDown(SDL_SCANCODE_2)) {
            player->Destroy();
        }
    }

    GameObject* go{FindGameObject(testGO)};
    if (go) {
        auto pos {go->GetComponent<Transform>().GetPosition()};
        auto apos {go->GetComponent<Transform>().GetAbsolutePosition()};
        ImGui::Begin("TestGO");
        ImGui::InputFloat3("position", glm::value_ptr(pos));
        ImGui::InputFloat3("a position", glm::value_ptr(apos));
        ImGui::End();
    }
}