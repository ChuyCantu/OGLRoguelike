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
    //+ Font Rendering Tests:
    TextRenderer::LoadFont("resources/assets/fonts/SourceCodePro-Regular.ttf", "SourceCode", 22, FontRenderMode::Raster);
    TextRenderer::LoadFont("resources/assets/fonts/Kenney Pixel Square.ttf", "KenneyPixel");
    TextRenderer::LoadFont("resources/assets/fonts/SHPinscher-Regular.otf", "SHPinscher");

    // AddGameObject<TilemapTest>();

    auto player {AddGameObject<BattlerPlayer>()};
    player->GetComponent<Transform>().SetPosition({-32.f, 16.f});
    // AddGameObject<BattlerEnemy>();

    auto tm {AddGameObject<GameObject>()};
    auto& tmComp {tm->AddCommponent<Tilemap>()};
    tmComp.chunksSize = 5;
    
    for (int y {-3}; y <= 3; ++y) {
        for (int x{-3}; x <= 3; ++x) {
            auto tile = MakeOwned<TileBase>();
            tile->sprite = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{32, 144}, glm::ivec2{16, 16});
            tmComp.SetTile(x * tmComp.chunksSize, y * tmComp.chunksSize, std::move(tile));
        }
    }
}

void TestScene2::LastUpdate() {
    TurnManager::Instance().Update();
}

void TestScene2::DebugGUI() {
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::SetNextWindowPos(ImVec2{2.f, 2.f}, 0, ImVec2{0.f, 0.f});
    ImGui::SetNextWindowSize(ImVec2{75, 85});
    ImGui::Begin("MousePosGrid", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
    glm::ivec2 mousePos {Input::GetMousePosition()};
    glm::ivec2 pos {Camera::GetMainCamera().ScreenToWorld2D(mousePos)};
    ImGui::Text("x: %i", pos.x / 16);
    ImGui::Text("y: %i", pos.y / 16);
    ImGui::PopStyleColor();
    ImGui::End();
}