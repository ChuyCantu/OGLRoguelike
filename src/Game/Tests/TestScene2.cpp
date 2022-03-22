#include "TestScene2.hpp"

#include "Battlers.hpp"
#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Core/Engine.hpp"
#include "Core/GameObject.hpp"
#include "Core/Tilemaps/Tilemap.hpp"
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

#include "Game/Algorithms.hpp"
#include "Game/DungeonGen/Dungeon.hpp"

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
static RandomTileData randomTile;
static AnimatedTileData animTile;
static AutotileData autoTile;

static TileBrush tileBrush;
Dungeon dungeonTest;

AStar pathfinding;
glm::ivec2 pathStart, pathEnd;

void TestScene2::Load() {
    //+ Font Rendering Tests:
    TextRenderer::LoadFont("resources/assets/fonts/SourceCodePro-Regular.ttf", "SourceCode", 22, FontRenderMode::Raster);
    TextRenderer::LoadFont("resources/assets/fonts/Kenney Pixel Square.ttf", "KenneyPixel");
    TextRenderer::LoadFont("resources/assets/fonts/SHPinscher-Regular.otf", "SHPinscher");

    auto player {AddGameObject<UnitPlayer>()};
    AddGameObject<UnitEnemy>();

    auto tm {AddGameObject<GameObject>()}; tm->name = "New Tilemap";
    tilemap = tm->Entity();
    auto& tmComp {tm->AddCommponent<Tilemap>()};
    tmComp.animationsDuration = 0.5f;
    tmComp.chunksSize = 24;
    tm->AddCommponent<TilemapCollider>().enabled = false;

    animTile.sprites.emplace_back(MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{32, 144}, glm::ivec2{16, 16}));
    animTile.sprites.emplace_back(MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 432}, glm::ivec2{16, 16}));

    tile0 = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 0}, glm::ivec2{16, 16});
    tile1 = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 144}, glm::ivec2{16, 16});
    tile2 = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 432}, glm::ivec2{16, 16});
    tile3 = MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 464}, glm::ivec2{16, 16});

    randomTile.sprites.emplace_back(MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 464}, glm::ivec2{16, 16}));
    randomTile.sprites.emplace_back(MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 432}, glm::ivec2{16, 16}));

    autoTile.rules.emplace(0, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{192, 48}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(1, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{160, 80}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(2, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{176, 64}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(3, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{112, 80}, glm::ivec2{16, 16})}});

    autoTile.rules.emplace(4, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{160, 48}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(5, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{160, 64}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(6, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{112, 48}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(7, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{112, 64}, glm::ivec2{16, 16})}});

    autoTile.rules.emplace(8, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{208, 64}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(9, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{144, 80}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(10, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{192, 64}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(11, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{128, 80}, glm::ivec2{16, 16})}});

    autoTile.rules.emplace(12, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{144, 48}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(13, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{144, 64}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(14, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{128, 48}, glm::ivec2{16, 16})}});
    autoTile.rules.emplace(15, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{128, 64}, glm::ivec2{16, 16})}});
    // autoTile.rules.emplace(15, TileRule{TileRule::Output::Animated, {MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{128, 64}, glm::ivec2{16, 16}),
    //                                                                  MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{128, 304}, glm::ivec2{16, 16})}});

    //+ Tile Brush
    tileBrush.CreateSimpleTile("tile0", MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16,   0}, glm::ivec2{16, 16}));
    tileBrush.CreateSimpleTile("tile1", MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 144}, glm::ivec2{16, 16}));
    tileBrush.CreateSimpleTile("tile2", MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 432}, glm::ivec2{16, 16}));
    tileBrush.CreateSimpleTile("tile3", MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 464}, glm::ivec2{16, 16}));

    tileBrush.CreateRandomTile("random0", MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{16, 464}, glm::ivec2{16, 16}), randomTile.sprites);

    tileBrush.CreateAnimatedTile("anim0", MakeRef<Sprite>(AssetManager::GetTexture("pit0_spritesheet"), glm::ivec2{32, 144}, glm::ivec2{16, 16}), animTile.sprites);

    tileBrush.CreateAutoTile("autotile0", MakeRef<Sprite>(AssetManager::GetTexture("floor_spritesheet"), glm::ivec2{192, 48}, glm::ivec2{16, 16}), autoTile.rules);
    tileBrush.GetTileAsset("autotile0")->layer = 1;

    // for (int y{-10}; y <= 10; ++y) {
    //     for (int x{-10}; x <= 10; ++x) {
    //         tileBrush.Paint(x, y, "tile0", tmComp);
    //     }
    // }

    dungeonTest.CreateNew({40, 40}, 5, 10, {5, 4}, {10, 6});
    // auto& rooms {dungeonTest.GetRooms()};
    // for (int i {0}; i < rooms.size(); ++i) {
    //     LOG_TRACE("room {}: {}, {},  {}, {}", i, rooms[i].position.x, rooms[i].position.y, rooms[i].size.x, rooms[i].size.y);
    // }

    // dungeonTest.GetNode(0, 0).type = NodeType::Ground;

    for (int y{0}; y < dungeonTest.GetSize().y; ++y) {
        for (int x{0}; x < dungeonTest.GetSize().x; ++x) {
            if (x == 0 || y == 0 || x == dungeonTest.GetSize().x - 1 || y == dungeonTest.GetSize().y - 1)
                tileBrush.Paint(x, y, "tile0", tmComp);

            DungeonNode& node {dungeonTest.GetNode(x, y)};
            if (node.type == NodeType::Air)
                tileBrush.Paint(x, y, "tile3", tmComp);
            if (node.type == NodeType::Wall)
                tileBrush.Paint(x, y, "tile0", tmComp);
            else if (node.type == NodeType::Ground)
                tileBrush.Paint(x, y, "autotile0", tmComp);
            else if (node.type == NodeType::NodeTypeCount)
                tileBrush.Paint(x, y, "tile1", tmComp);
        }
    }

    // AStar pathfinding{dungeonTest.GetSize().x, dungeonTest.GetSize().y};
    // std::vector<glm::ivec2> path;
    // if (pathfinding.FindPath({0, 0}, {10, 10}, path)) {
    //     for (auto& pos : path) {
    //         tileBrush.Paint(pos.x, pos.y, "anim0", tmComp);
    //     }

    //     // for (size_t x {0}; x < pathfinding.GetSize().x; ++x) {
    //     //     for (size_t y {0}; y < pathfinding.GetSize().y; ++y) {
    //     //         auto& node {pathfinding.GetNode({x, y})};
    //     //         if (node.visited)
    //     //             tileBrush.Paint(x, y, "anim0", tmComp);
    //     //     }
    //     // }
    // }

    pathfinding.CreateMap(dungeonTest.GetSize().x, dungeonTest.GetSize().y);
    
    for (int y{0}; y < dungeonTest.GetSize().y; ++y) {
        for (int x{0}; x < dungeonTest.GetSize().x; ++x) {
            if (x == 0 || y == 0 || x == dungeonTest.GetSize().x - 1 || y == dungeonTest.GetSize().y - 1)
                tileBrush.Paint(x, y, "tile0", tmComp);

            DungeonNode& node {dungeonTest.GetNode(x, y)};
            if (node.type == NodeType::Air)
                pathfinding.GetNode({x, y}).isObstacle = true;
            if (node.type == NodeType::Wall)
                pathfinding.GetNode({x, y}).isObstacle = true;
            else if (node.type == NodeType::Ground)
                pathfinding.GetNode({x, y}).cost = 1;
        }
    }
}

void TestScene2::LastUpdate() {
    TurnManager::Instance().Update();

    // for (auto& room1 : dungeonTest.GetRooms()) {
    //     engine->GetRenderer()->DrawLine2D(room1.position * 16.f, room1.position * 16.f + 16.f, Color2Vec3(ColorNames::blue));

    //     for (auto& room2 : dungeonTest.GetRooms()) {
    //         glm::vec2 room1center {room1.position + room1.size / 2.f};
    //         glm::vec2 room2center {room2.position + room2.size / 2.f};
    //         engine->GetRenderer()->DrawLine2D(room1center * 16.f, room2center * 16.f, Color2Vec3(ColorNames::lime));
    //     }
    // }

    //+ Triangles
    // for (size_t i = 0; i < dungeonTest.triangles.size(); i += 3) {
    //     glm::vec2 p0{dungeonTest.roomsCenterCoords[2 * dungeonTest.triangles[i]], 
    //                  dungeonTest.roomsCenterCoords[2 * dungeonTest.triangles[i] + 1]};
    //     glm::vec2 p1{dungeonTest.roomsCenterCoords[2 * dungeonTest.triangles[i + 1]],
    //                  dungeonTest.roomsCenterCoords[2 * dungeonTest.triangles[i + 1] + 1]};
    //     glm::vec2 p2{dungeonTest.roomsCenterCoords[2 * dungeonTest.triangles[i + 2]],
    //                  dungeonTest.roomsCenterCoords[2 * dungeonTest.triangles[i + 2] + 1]};
    //     engine->GetRenderer()->DrawLine2D(p0 * 16.f, p1 * 16.f, Color2Vec3(ColorNames::red));
    //     engine->GetRenderer()->DrawLine2D(p1 * 16.f, p2 * 16.f, Color2Vec3(ColorNames::red));
    //     engine->GetRenderer()->DrawLine2D(p2 * 16.f, p0 * 16.f, Color2Vec3(ColorNames::red));
    // }

    //+ Minimum Spaning Tree
    // for (auto& [v1, v2] : dungeonTest.connections) {
    //     engine->GetRenderer()->DrawLine2D(v1 * 16.f /*+ glm::vec2{0.8f}*/, v2 * 16.f /*+ glm::vec2{0.8f}*/, Color2Vec3(ColorNames::lime));
    // }
    
    if (Input::GetKeyDown(SDL_SCANCODE_0)) {
        auto& tmComp = FindGameObject(tilemap)->GetComponent<Tilemap>();

        dungeonTest.CreateNew({40, 40}, 5, 10, {5, 4}, {10, 6});
        for (int y{0}; y < dungeonTest.GetSize().y; ++y) {
            for (int x{0}; x < dungeonTest.GetSize().x; ++x) {
                if (x == 0 || y == 0 || x == dungeonTest.GetSize().x - 1 || y == dungeonTest.GetSize().y - 1)
                    tileBrush.Paint(x, y, "tile0", tmComp);

                DungeonNode& node{dungeonTest.GetNode(x, y)};
                if (node.type == NodeType::Air)
                    tileBrush.Paint(x, y, "tile3", tmComp);
                if (node.type == NodeType::Wall)
                    tileBrush.Paint(x, y, "tile0", tmComp);
                else if (node.type == NodeType::Ground)
                    tileBrush.Paint(x, y, "autotile0", tmComp);
                else if (node.type == NodeType::NodeTypeCount)
                    tileBrush.Paint(x, y, "tile1", tmComp);
            }
        }
    }
    
    // engine->GetRenderer()->DrawLine2D(glm::vec2{0.f, 0.f} * 16.f, glm::vec2{5.f, 5.f} * 16.f, Color2Vec3(ColorNames::red));
    // engine->GetRenderer()->DrawLine2D(glm::vec2{0.f, 0.f} * 16.f, glm::vec2{-5.f, -5.f} * 16.f, Color2Vec3(ColorNames::blue));
    // engine->GetRenderer()->DrawLine2D(glm::vec2{0.f, 0.f} * 16.f, glm::vec2{-5.f, 5.f} * 16.f, Color2Vec3(ColorNames::lime));

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
        auto& tmComp {tm->GetComponent<Tilemap>()};

        if (Input::GetKey(SDL_SCANCODE_LCTRL) && Input::GetMouseButtonDown(SDL_BUTTON_LEFT)) {
            glm::ivec2 mousePos{Input::GetMousePosition()};
            glm::ivec2 pos{Camera::GetMainCamera().ScreenToWorld2D(mousePos)};
            glm::ivec2 tiledPos{pos / 16};
            if (pos.x < 0) --tiledPos.x;
            if (pos.y < 0) --tiledPos.y;

            // Repaint map
            for (int y{0}; y < dungeonTest.GetSize().y; ++y) {
                for (int x{0}; x < dungeonTest.GetSize().x; ++x) {
                    if (x == 0 || y == 0 || x == dungeonTest.GetSize().x - 1 || y == dungeonTest.GetSize().y - 1)
                        tileBrush.Paint(x, y, "tile0", tmComp);

                    DungeonNode& node{dungeonTest.GetNode(x, y)};
                    if (node.type == NodeType::Air)
                        tileBrush.Paint(x, y, "tile3", tmComp);
                    if (node.type == NodeType::Wall)
                        tileBrush.Paint(x, y, "tile0", tmComp);
                    else if (node.type == NodeType::Ground)
                        tileBrush.Paint(x, y, "autotile0", tmComp);
                    else if (node.type == NodeType::NodeTypeCount)
                        tileBrush.Paint(x, y, "tile1", tmComp);
                }
            }

            tileBrush.Paint(tiledPos.x, tiledPos.y, "anim0", tmComp);
            pathStart = tiledPos;
        }
        if (Input::GetKey(SDL_SCANCODE_LSHIFT) && Input::GetMouseButtonDown(SDL_BUTTON_LEFT)) {
            glm::ivec2 mousePos{Input::GetMousePosition()};
            glm::ivec2 pos{Camera::GetMainCamera().ScreenToWorld2D(mousePos)};
            glm::ivec2 tiledPos{pos / 16};
            if (pos.x < 0) --tiledPos.x;
            if (pos.y < 0) --tiledPos.y;

            tileBrush.Paint(tiledPos.x, tiledPos.y, "anim0", tmComp);
            pathEnd = tiledPos;

            std::vector<glm::ivec2> path;
            if (pathfinding.FindPath(pathStart, pathEnd, path, true)) {
                // Paint path
                for (auto& pos : path) {
                    tileBrush.Paint(pos.x, pos.y, "anim0", tmComp);
                }
            }
        }

        static bool isRepeating {false};
        if (Input::GetMouseButton(SDL_BUTTON_RIGHT)) {
            static glm::ivec2 prevPos {999999, 999999};
            glm::ivec2 mousePos{Input::GetMousePosition()};
            glm::ivec2 pos{Camera::GetMainCamera().ScreenToWorld2D(mousePos)};
            glm::ivec2 tiledPos{pos / 16};
            if (pos.x < 0) --tiledPos.x;
            if (pos.y < 0) --tiledPos.y;

            if (isRepeating && prevPos == tiledPos)
                return;
            isRepeating = true;
            prevPos = tiledPos;

            switch (tileID) {
                case -1: {
                    tmComp.SetTile(tiledPos.x, tiledPos.y, nullptr);
                    break;
                }
                case 0: {           
                    tileBrush.Paint(tiledPos.x, tiledPos.y, "tile0", tmComp);      
                    break;
                }
                case 1: {
                    tileBrush.Paint(tiledPos.x, tiledPos.y, "tile1", tmComp); 
                    break;
                }
                case 2: {
                    tileBrush.Paint(tiledPos.x, tiledPos.y, "tile2", tmComp); 
                    break;
                }
                case 3: {
                    tileBrush.Paint(tiledPos.x, tiledPos.y, "tile3", tmComp); 
                    break;
                }
                case 4: {
                    tileBrush.Paint(tiledPos.x, tiledPos.y, "random0", tmComp);
                    break;
                }
                case 5: {
                    tileBrush.Paint(tiledPos.x, tiledPos.y, "anim0", tmComp); 
                    break;
                }
                case 6: {
                    tileBrush.Paint(tiledPos.x, tiledPos.y, "autotile0", tmComp); 
                    break;
                }
                default:
                    break;
            }                
        }
        if (Input::GetMouseButtonUp(SDL_BUTTON_RIGHT)) {
            isRepeating = false;
        }

        if (Input::GetKeyDown(SDL_SCANCODE_KP_0)) {
            tm->GetComponent<Tilemap>().Clear();
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
    if (ImGui::ImageButton((ImTextureID)tileBrush.GetTileAsset("tile0")->defaultSprite->GetTexture()->GetID(), {64, 64}, {tile0->GetMinUV().x, 1.0f - tile0->GetMinUV().y}, {tile0->GetMaxUV().x, 1.0f - tile0->GetMaxUV().y})) {
        tileID = 0;
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tileBrush.GetTileAsset("tile1")->defaultSprite->GetTexture()->GetID(), {64, 64}, {tile1->GetMinUV().x, 1.0f - tile1->GetMinUV().y}, {tile1->GetMaxUV().x, 1.0f - tile1->GetMaxUV().y})) {
        tileID = 1;
    }
    ImGui::PopID();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tileBrush.GetTileAsset("tile2")->defaultSprite->GetTexture()->GetID(), {64, 64}, {tile2->GetMinUV().x, 1.0f - tile2->GetMinUV().y}, {tile2->GetMaxUV().x, 1.0f - tile2->GetMaxUV().y})) {
        tileID = 2;
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tileBrush.GetTileAsset("tile3")->defaultSprite->GetTexture()->GetID(), {64, 64}, {tile3->GetMinUV().x, 1.0f - tile3->GetMinUV().y}, {tile3->GetMaxUV().x, 1.0f - tile3->GetMaxUV().y})) {
        tileID = 3;
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tileBrush.GetTileAsset("random0")->defaultSprite->GetTexture()->GetID(), {64, 64}, {tile3->GetMinUV().x, 1.0f - tile3->GetMinUV().y}, {tile3->GetMaxUV().x, 1.0f - tile3->GetMaxUV().y})) {
        tileID = 4;
    }
    ImGui::PopID();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tileBrush.GetTileAsset("anim0")->defaultSprite->GetTexture()->GetID(), {64, 64}, {tile2->GetMinUV().x, 1.0f - tile2->GetMinUV().y}, {tile2->GetMaxUV().x, 1.0f - tile2->GetMaxUV().y})) {
        tileID = 5;
    }
    ImGui::PopID();
    ImGui::PushID(uid++);
    if (ImGui::ImageButton((ImTextureID)tileBrush.GetTileAsset("autotile0")->defaultSprite->GetTexture()->GetID(), {64, 64}, {tileBrush.GetTileAsset("autotile0")->defaultSprite->GetMinUV().x, 1.0f - tileBrush.GetTileAsset("autotile0")->defaultSprite->GetMinUV().y}, {tileBrush.GetTileAsset("autotile0")->defaultSprite->GetMaxUV().x, 1.0f - tileBrush.GetTileAsset("autotile0")->defaultSprite->GetMaxUV().y})) {
        tileID = 6;
    }
    ImGui::PopID();
    ImGui::InputInt("tileID", &tileID);

    ImGui::GetWindowSize();
    ImGui::End();
}