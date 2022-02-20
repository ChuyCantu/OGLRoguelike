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

    AddGameObject<TilemapTest>();

    auto player {AddGameObject<BattlerPlayer>()};
    player->GetComponent<Transform>().SetPosition({-32.f, 16.f});
    AddGameObject<BattlerEnemy>();

    auto tm {AddGameObject<GameObject>()};
    // tm->AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{48, 112}, glm::ivec2{16, 16}), ColorNames::white, 10);
    // tm->GetComponent<Transform>().SetPosition(glm::vec2{-16.f});
    auto& tmComp {tm->AddCommponent<Tilemap>()};
    tmComp.tileSize = 16;
    tmComp.chunksSize = 5;
    auto tile {MakeOwned<TileBase>()};
    tile->sprite = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{0, 0}, glm::ivec2{16, 16});
    tmComp.SetTile(-1, -1, std::move(tile));

    tile = MakeOwned<TileBase>();
    tile->sprite = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 0}, glm::ivec2{16, 16});
    tmComp.SetTile(-4, 0, std::move(tile));

    tile = MakeOwned<TileBase>();
    tile->sprite = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{32, 0}, glm::ivec2{16, 16});
    tmComp.SetTile(-2, 0, std::move(tile));

    tile = MakeOwned<TileBase>();
    tile->sprite = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{32, 0}, glm::ivec2{16, 16});
    tmComp.SetTile(-5, 0, std::move(tile));

    tile = MakeOwned<TileBase>();
    tile->sprite = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{32, 0}, glm::ivec2{16, 16});
    tmComp.SetTile(-6, 5, std::move(tile));
}

void TestScene2::LastUpdate() {
    TurnManager::Instance().Update();
}

void TestScene2::DebugGUI() {

}