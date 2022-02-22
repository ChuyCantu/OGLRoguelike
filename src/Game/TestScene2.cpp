#include "TestScene2.hpp"

#include "Battlers.hpp"
#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Core/Engine.hpp"
#include "Core/GameObject.hpp"
#include "Rendering/Sprite.hpp"
#include "PlayerTest.hpp"
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

entt::entity tilemap {entt::null};
static int tileID {-1};
static Ref<Sprite> tile0;
static Ref<Sprite> tile1;
static Ref<Sprite> tile2;
static Ref<Sprite> tile3;

void TestScene2::Load() {
    //+ Font Rendering Tests:
    TextRenderer::LoadFont("resources/assets/fonts/SourceCodePro-Regular.ttf", "SourceCode", 22, FontRenderMode::Raster);
    TextRenderer::LoadFont("resources/assets/fonts/Kenney Pixel Square.ttf", "KenneyPixel");
    TextRenderer::LoadFont("resources/assets/fonts/SHPinscher-Regular.otf", "SHPinscher");

    auto player {AddGameObject<BattlerPlayer>()};
    // AddGameObject<BattlerEnemy>();

    auto tm {AddGameObject<GameObject>()}; tm->name = "New Tilemap";
    tilemap = tm->Entity();
    auto& tmComp {tm->AddCommponent<Tilemap>()};
    tmComp.chunksSize = 5;
    tm->AddCommponent<TilemapCollider>();
    
    for (int y {-3}; y <= 3; ++y) {
        for (int x{-3}; x <= 3; ++x) {
            auto tile = MakeOwned<Tile>();
            tile->sprite = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{32, 144}, glm::ivec2{16, 16});
            tmComp.SetTile(x * tmComp.chunksSize, y * tmComp.chunksSize, std::move(tile));
        }
    }

    tile0 = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 0}, glm::ivec2{16, 16});
    tile1 = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 144}, glm::ivec2{16, 16});
    tile2 = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 432}, glm::ivec2{16, 16});
    tile3 = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 464}, glm::ivec2{16, 16});
}

void TestScene2::LastUpdate() {
    TurnManager::Instance().Update();

    if (Input::GetKeyDown(SDL_SCANCODE_G)) {
        auto tm = FindGameObject(tilemap);
        if (tm)
            tm->GetComponent<TilemapCollider>().isSolid = !tm->GetComponent<TilemapCollider>().isSolid;
    }

    if (Input::GetKeyDown(SDL_SCANCODE_KP_6)) {
        auto tm = FindGameObject(tilemap);
        if (tm)
            tm->GetComponent<Tilemap>().enabled = !tm->GetComponent<Tilemap>().enabled;
    }

    auto tm {FindGameObject(tilemap)};
    if (tm) {
        if (Input::GetMouseButtonDown(SDL_BUTTON_RIGHT)) {
            glm::ivec2 mousePos{Input::GetMousePosition()};
            glm::ivec2 pos{Camera::GetMainCamera().ScreenToWorld2D(mousePos)};
            glm::ivec2 tiledPos{pos / 16};
            if (pos.x < 0) --tiledPos.x;
            if (pos.y < 0) --tiledPos.y;

            auto& tmComp {tm->GetComponent<Tilemap>()};
            switch (tileID) {
                case -1: {
                    tmComp.SetTile(tiledPos.x, tiledPos.y, nullptr);
                    break;
                }
                case 0: {
                    auto tile = MakeOwned<Tile>();
                    tile->sprite = tile0;
                    tmComp.SetTile(tiledPos.x, tiledPos.y, std::move(tile));                    
                    break;
                }
                case 1: {
                    auto tile = MakeOwned<Tile>();
                    tile->sprite = tile1;
                    tmComp.SetTile(tiledPos.x, tiledPos.y, std::move(tile));
                    break;
                }
                case 2: {
                    auto tile = MakeOwned<Tile>();
                    tile->sprite = tile2;
                    tmComp.SetTile(tiledPos.x, tiledPos.y, std::move(tile));
                    break;
                }
                case 3: {
                    auto tile = MakeOwned<Tile>();
                    tile->sprite = tile3;
                    tmComp.SetTile(tiledPos.x, tiledPos.y, std::move(tile));
                    break;
                }
                default:
                    break;
            }                
        }
    }
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
    glm::ivec2 tiledPos {pos / 16};
    if (pos.x < 0) --tiledPos.x;
    if (pos.y < 0) --tiledPos.y;
    ImGui::Text("x: %i", tiledPos.x);
    ImGui::Text("y: %i", tiledPos.y);
    ImGui::PopStyleColor();
    ImGui::End();

    
    ImGui::Begin("ImageButtonTest");
    auto tm {FindGameObject(tilemap)};
    int uid {0};
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)AssetManager::GetTexture("default")->GetID(), {64, 64})) {
       tileID = -1;
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tile0->GetTexture()->GetID(), {64, 64}, {tile0->GetMinUV().x, 1.0f - tile0->GetMinUV().y}, {tile0->GetMaxUV().x, 1.0f - tile0->GetMaxUV().y})) {
        tileID = 0;
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tile1->GetTexture()->GetID(), {64, 64}, {tile1->GetMinUV().x, 1.0f - tile1->GetMinUV().y}, {tile1->GetMaxUV().x, 1.0f - tile1->GetMaxUV().y})) {
        tileID = 1;
    }
    ImGui::PopID();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tile2->GetTexture()->GetID(), {64, 64}, {tile2->GetMinUV().x, 1.0f - tile2->GetMinUV().y}, {tile2->GetMaxUV().x, 1.0f - tile2->GetMaxUV().y})) {
        tileID = 2;
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tile3->GetTexture()->GetID(), {64, 64}, {tile3->GetMinUV().x, 1.0f - tile3->GetMinUV().y}, {tile3->GetMaxUV().x, 1.0f - tile3->GetMaxUV().y})) {
        tileID = 3;
    }
    ImGui::PopID();
    ImGui::InputInt("tileID", &tileID);

    ImGui::GetWindowSize();
    ImGui::End();
}