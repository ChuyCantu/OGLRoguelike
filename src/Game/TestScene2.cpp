#include "TestScene2.hpp"

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

TestScene2::TestScene2(Engine* engine) 
    : Scene{engine} {
    Load();
}

TestScene2::~TestScene2() {
    TurnManager::Instance().Clear();
}


void TestScene2::Load() {
    AddGameObject<TilemapTest>();

    auto player {AddGameObject<BattlerPlayer>()};
    player->GetComponent<Transform>().SetPosition({-32.f, 16.f});
    AddGameObject<BattlerEnemy>();

    //+ Font Rendering Tests:
    TextRenderer::LoadFont("resources/assets/fonts/SourceCodePro-Regular.ttf", "SourceCode", 22, FontRenderMode::Raster);
    TextRenderer::LoadFont("resources/assets/fonts/Kenney Pixel Square.ttf", "KenneyPixel");
    TextRenderer::LoadFont("resources/assets/fonts/SHPinscher-Regular.otf", "SHPinscher");

    auto panel {UI::AddPanel(MakeOwned<Panel>())};
    auto widget = panel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.f}, glm::vec2{32.f}},
                                                    MakeRef<Sprite>(AssetManager::GetTexture("gui0"), glm::ivec2{64, 160}, glm::ivec2{16}),
                                                    "TestImage"));
    widget->SetAnchor(Anchor::Top);
    widget->SetPivot(glm::vec2{0.5f, 0.0f});
    widget->SetPosition(glm::vec2{0.f, 16.f});
}

void TestScene2::LastUpdate() {
    TurnManager::Instance().Update();
}

void TestScene2::DebugGUI() {
    auto image {UI::Stack()->FindChild("TestImage", true)};
    if (image) {
        DebugWidgetWindow(image, image->name);
    }
}