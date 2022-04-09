#include "Player.hpp"

#include "Core/Tilemaps/Tilemap.hpp"
#include "Core/Time.hpp"
#include "Game/Action.hpp"
#include "Game/Constants.hpp"
#include "Game/DungeonGen/Dungeon.hpp"
#include "Input/Input.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Utils/Random.hpp"

#ifdef TEST_MINIMAP
#include "UI/Image.hpp"
#include "UI/UI.hpp"
#include "UI/Panel.hpp"
#endif  // TEST_MINIMAP

#include <imgui.h>


static entt::entity floorE {entt::null};
static entt::entity wallsE {entt::null};

Player::Player(Scene* scene, const std::string& name) : Unit{scene, name} {
    tag = "Player";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 0}, glm::ivec2{16, 16}), ColorNames::white, 0)};
    sr.pivot = glm::vec2{0.0f, -0.25f};

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.25f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.25f});

    AddCommponent<Collider>();

    // AddCommponent<MoveComponent>().Teleport(glm::vec3{0.f, 0.f, 0.0f});

    AddCommponent<UnitComponent>(1, 10, 0, 100);

    // Camera::GetMainCamera().SetScale(2);

    //+ -------------
    floorE = FindGameObject("Dungeon Floor")->Entity();
    wallsE = FindGameObject("Dungeon Walls")->Entity();
}

void Player::Start() {
    // fov.CreateMap(dungeon->GetSize());

    // for (int y{0}; y < dungeon->GetSize().y; ++y) {
    //     for (int x{0}; x < dungeon->GetSize().x; ++x) {
    //         DungeonNode& node{dungeon->GetNode(x, y)};
    //         FovNode& fNode{fov.GetNode(x, y)};
    //         switch (node.type) {
    //             case NodeType::Ground: {
    //                 fNode.blocksLight = false;
    //                 break;
    //             }
    //             case NodeType::Wall: {
    //                 fNode.blocksLight = true;
    //                 break;
    //             }
    //         }
    //     }
    // }

#ifdef TEST_MINIMAP
    minimap = MakeRef<Texture>();
    uint8_t minimapStartData[100*100*4] {};
    for (size_t i {0}; i < 100*100*4; i+=4) {
        minimapStartData[i] = 0;
        minimapStartData[i + 1] = 0;
        minimapStartData[i + 2] = 0;
        minimapStartData[i + 3] = 255;
    }
    minimap->Generate(100, 100, &minimapStartData, TextureFormat::RGBA8, TextureFormat::RGBA);
    minimap->SetMinFilter(TextureParameter::Nearest)
            .SetMagFilter(TextureParameter::Nearest)
            .SetWrapS(TextureParameter::ClampToEdge)
            .SetWrapT(TextureParameter::ClampToEdge);

    auto panel {UI::AddPanel(MakeOwned<Panel>())};
    auto image {panel->AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f}, glm::vec2{200.0f}}, 
                                                 MakeRef<Sprite>(minimap)))};
    image->SetAnchor(Anchor::BottomRight);
    image->SetPivot(glm::vec2{1.0f});
    image->SetPosition(glm::vec2{0.0f});

    for (int y{0}; y < dungeon->GetSize().y; ++y) {
        for (int x{0}; x < dungeon->GetSize().x; ++x) {
            DungeonNode& node{dungeon->GetNode(x, y)};
            if (node.type == NodeType::Wall) {
                size_t idx {((x + 30) + (y + 30) * 100) * static_cast<size_t>(4)};
                minimapStartData[idx + 2] = 255;
            }
            else if (node.type == NodeType::Ground) {
                size_t idx {((x + 30) + (y + 30) * 100) * static_cast<size_t>(4)};
                minimapStartData[idx + 2] = 155;
            }
        }   
    }
    minimap->SubImage(0, 0, 100, 100, &minimapStartData);
#endif  // TEST_MINIMAP
}

void Player::Update() {
    // stateMachine.Update();

    auto& transform{GetComponent<Transform>()};
    
    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        if (Input::GetKeyDown(SDL_SCANCODE_UP)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + vec3::up, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + vec3::up, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_DOWN)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + vec3::down, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + vec3::down, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_LEFT)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + vec3::left, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + vec3::left, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_RIGHT)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + vec3::right, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + vec3::right, .15f);
        }

        if (Input::GetKeyDown(SDL_SCANCODE_U)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
        }

        if (Input::GetKeyDown(SDL_SCANCODE_KP_8)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + vec3::up, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + vec3::up, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_2)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + vec3::down, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + vec3::down, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_4)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + vec3::left, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + vec3::left, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_6)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + vec3::right, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + vec3::right, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_7)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + glm::vec3{-1.0f, 1.0f, 0.0f}, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + glm::vec3{-1.0f, 1.0f, 0.0f}, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_9)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + glm::vec3{1.0f, 1.0f, 0.0f}, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + glm::vec3{1.0f, 1.0f, 0.0f}, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_3)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + glm::vec3{1.0f, -1.0f, 0.0f}, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + glm::vec3{1.0f, -1.0f, 0.0f}, .15f);
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_1)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() / TILE_SIZEF + glm::vec3{-1.0f, -1.0f, 0.0f}, .15f, dungeon));
            MoveToTile(transform.GetPosition() / TILE_SIZEF + glm::vec3{-1.0f, -1.0f, 0.0f}, .15f);
        }
    }

    if (GetComponent<MoveComponent>().IsMoving()) {
        MoveCamera();
    }
    // Camera::GetMainCamera().SetPosition(transform.GetPosition());

    // Debug
    glm::ivec2 mousePos{Input::GetMousePosition()};
    glm::ivec2 pos{Camera::GetMainCamera().ScreenToWorld2D(mousePos)};
    glm::ivec2 tiledPos{pos / 16};
    if (pos.x < 0) --tiledPos.x;
    if (pos.y < 0) --tiledPos.y;
    if (Input::GetMouseButtonDown(SDL_BUTTON_LEFT)) {
        DungeonNode* node {dungeon->TryGetNode(tiledPos.x, tiledPos.y)};
        if (node)
            node->cost = std::max(1, node->cost - 1);
        AStar::Node* anode {dungeon->pathfinding.TryGetNode(tiledPos)};
        if (anode)
            anode->cost = std::max(1, anode->cost - 1);
    }
    if (Input::GetMouseButtonDown(SDL_BUTTON_RIGHT)) {
        DungeonNode* node {dungeon->TryGetNode(tiledPos.x, tiledPos.y)};
        if (node)
            ++node->cost;
        AStar::Node* anode {dungeon->pathfinding.TryGetNode(tiledPos)};
        if (anode)
            ++anode->cost;
    }
    if (Input::GetMouseButtonDown(SDL_BUTTON_MIDDLE)) {
        AStar::Node* anode {dungeon->pathfinding.TryGetNode(tiledPos)};
        if (anode)
            anode->isObstacle = true;
    }
}

void Player::OnCollisionEnter(const Collider& other) {
    Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());

    if (other.gameobject->tag == "Enemy")
        LOG_TRACE("Attacked by {}", other.gameobject->tag);
}

void Player::OnCollisionStay(const Collider& other) {
    
}

void Player::OnCollisionExit(const Collider& other) {
    
}

void Player::SetStartPosition(const glm::ivec2 position) {
    Unit::SetStartPosition(position);
    Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());

    UpdateFOV();
}

void Player::MoveToTile(const glm::ivec2& dest, float duration) {
    if (dungeon->GetNode(dest.x, dest.y).unit || dungeon->pathfinding.GetNode(dest).isObstacle)
        return;

    auto& unitPos {GetComponent<UnitComponent>().GetPosition()};
    auto newNormalizedDest {dest - unitPos};

    if (newNormalizedDest.x != 0 && newNormalizedDest.y != 0) {  // diagonal move
        if (GetDiagonalMovementType() == DiagonalMovement::Never)
            return;

        // ------------------
        if (GetDiagonalMovementType() != DiagonalMovement::Always) {
            // check if adjacent nodes are obstacles
            AStar::Node& n1{dungeon->pathfinding.GetNode(glm::ivec2{unitPos.x, dest.y})};
            AStar::Node& n2{dungeon->pathfinding.GetNode(glm::ivec2{dest.x, unitPos.y})};

            switch (GetDiagonalMovementType()) {
                case DiagonalMovement::AllowOneObstacle:
                    if (n1.isObstacle && n2.isObstacle)
                        return;
                    break;
                case DiagonalMovement::OnlyWhenNoObstacles:
                    if (n1.isObstacle || n2.isObstacle)
                        return;
                    break;
            }
        }
    }

    GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, dest, duration, dungeon));
    cameraSrcPos = GetComponent<Transform>().GetPosition();
    cameraDestPos = glm::vec3 {dest.x * TILE_SIZEF, dest.y * TILE_SIZEF, 0.0f};
    cameraMoveTimer = 0.0f;
    cameraMoveDuration = duration;

    UpdateFOV();
}

void Player::MoveCamera() {
    if (cameraMoveTimer <= cameraMoveDuration) {
        Camera::GetMainCamera().SetPosition(Lerp(cameraSrcPos, cameraDestPos, cameraMoveTimer / cameraMoveDuration));
        cameraMoveTimer += Time::deltaTime;
    } else {
        Camera::GetMainCamera().SetPosition(cameraDestPos);
    }
}

void Player::UpdateFOV() {
    //+ Clear previous fov:
    
    // TODO: Fix min and max affected tiles not clearing properly
    // for (int y{dungeon->fov.minAffectedTile.y}; y < dungeon->fov.maxAffectedTile.y; ++y) {
    //     for (int x{dungeon->fov.minAffectedTile.x}; x < dungeon->fov.maxAffectedTile.x; ++x) {
    //         FovNode& fNode{dungeon->fov.GetNode(x, y)};
    //         fNode.visible = false;
    //         if (fNode.revealed)
    //             fNode.lightLevel = REVEALED_LIGHT_LEVEL;
    //         else
    //             fNode.lightLevel = 0.0f;
    //     }
    // }

    for (int y {0}; y < dungeon->fov.GetSize().y; ++y) {
        for (int x {0}; x < dungeon->fov.GetSize().x; ++x) {
            FovNode& fNode {dungeon->fov.GetNode(x, y)};
            fNode.visible = false;
            if (fNode.revealed)
                fNode.lightLevel = REVEALED_LIGHT_LEVEL;
            else    
                fNode.lightLevel = 0.0f;
        }
    }

    //+ Calculate fov:
    dungeon->fov.Compute(GetComponent<UnitComponent>().GetPosition(), 10, 2, FovType::Permissive);

    for (int y {0}; y < dungeon->fov.GetSize().y; ++y) {
        for (int x {0}; x < dungeon->fov.GetSize().x; ++x) {
            DungeonNode& node {dungeon->GetNode(x, y)};
            FovNode& fNode {dungeon->fov.GetNode(x, y)};
            switch (node.type) {
                case NodeType::Ground: {
                    if (fNode.visible) {
                        auto tile{FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(x, y)};
                        float light {fNode.lightLevel};
                        tile->color = Color{light, light, light};
                    }
                    else {
                        if (fNode.revealed) {
                            auto tile{FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(x, y)};
                            float light {REVEALED_LIGHT_LEVEL};
                            tile->color = Color(light, light, light);
                        }
                        else {
                            auto tile{FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(x, y)};
                            tile->color = ColorNames::black;
                        }
                    }
                    break;
                }
                case NodeType::Wall: {
                    if (fNode.visible) {
                        auto tile{FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(x, y)};
                        float light {fNode.lightLevel};
                        tile->color = Color{light, light, light};
                    } 
                    else {
                        if (fNode.revealed) {
                            auto tile{FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(x, y)};
                            float light {REVEALED_LIGHT_LEVEL};
                            tile->color = Color(light, light, light);
                        }
                        else {
                            auto tile{FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(x, y)};
                            tile->color = ColorNames::black;
                        }
                    }
                    break;
                }
            }
        }
    }
}

static float changeTileTimer {0};
void Player::DebugGUI() {
    auto& unit {GetComponent<UnitComponent>()};
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::SetNextWindowPos(ImVec2{2.f, 2.f}, 0, ImVec2{0.f, 0.f});
    // ImGui::SetNextWindowSize(ImVec2{75, 85});
    ImGui::SetNextWindowSize(ImVec2{200, 85});
    ImGui::Begin("MousePosGrid", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
    glm::ivec2 mousePos{Input::GetMousePosition()};
    glm::ivec2 pos{Camera::GetMainCamera().ScreenToWorld2D(mousePos)};
    glm::ivec2 tiledPos{pos / 16};
    if (pos.x < 0) --tiledPos.x;
    if (pos.y < 0) --tiledPos.y;
    ImGui::Text("x: %i", tiledPos.x);
    ImGui::Text("y: %i", tiledPos.y);
    int cost {0};
    // DungeonNode* dnode {dungeon->TryGetNode(tiledPos.x, tiledPos.y)};
    // if (dnode && dnode->unit)
    //     ImGui::Text("u?: true");
    // else
    //     ImGui::Text("u?: false");
    FovNode* dnode {dungeon->fov.TryGetNode(tiledPos.x, tiledPos.y)};
    if (dnode)
        // ImGui::Text("l: %i", dnode->lightLevel);
        // ImGui::Text("l: %f, d: %f", dnode->lightLevel, glm::distance(glm::vec2{unit.GetPosition()}, glm::vec2{tiledPos}));
        if (dnode->visible)
            ImGui::Text("v:%s, l:%f", "true", dnode->lightLevel);
        else
            ImGui::Text("v:%s, l:%f", "false", dnode->lightLevel);
    else
        ImGui::Text("l: 0, d: ?");
    ImGui::PopStyleColor();
    ImGui::End();
    
    ImGui::Begin("Player");
    ImGui::Text("pos: %i, %i", unit.GetPosition().x, unit.GetPosition().y);
    ImGui::End();

    // if (Input::GetMouseButtonDown(SDL_BUTTON_LEFT)) {
    //     DungeonNode* node {dungeon->TryGetNode(tiledPos.x, tiledPos.y)};
    //     if (node) {
    //         switch (node->type) {
    //             case NodeType::Ground: {
    //                 auto tile {FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(tiledPos.x, tiledPos.y)};
    //                 auto& tileColor {Color2Vec3(tile->color)};
    //                 glm::vec3 color {tileColor - 0.1f};
    //                 tile->color = Vec3ToColor(color);
    //                 break;
    //             }
    //             case NodeType::Wall: {
    //                 auto tile {FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(tiledPos.x, tiledPos.y)};
    //                 auto& tileColor {Color2Vec3(tile->color)};
    //                 glm::vec3 color {tileColor - 0.1f};
    //                 tile->color = Vec3ToColor(color);
    //                 break;
    //             }
    //         }
    //     }
    // }
    // if (Input::GetMouseButtonDown(SDL_BUTTON_RIGHT)) {
    //     DungeonNode* node {dungeon->TryGetNode(tiledPos.x, tiledPos.y)};
    //     if (node) {
    //         switch (node->type) {
    //             case NodeType::Ground: {
    //                 auto tile {FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(tiledPos.x, tiledPos.y)};
    //                 auto& tileColor {Color2Vec3(tile->color)};
    //                 glm::vec3 color {tileColor + 0.1f};
    //                 tile->color = Vec3ToColor(color);
    //                 break;
    //             }
    //             case NodeType::Wall: {
    //                 auto tile {FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(tiledPos.x, tiledPos.y)};
    //                 auto& tileColor {Color2Vec3(tile->color)};
    //                 glm::vec3 color {tileColor + 0.1f};
    //                 tile->color = Vec3ToColor(color);
    //                 break;
    //             }
    //         }
    //     }
    // }

    changeTileTimer += Time::deltaTime;
    if (changeTileTimer < 0.5f) return;

    changeTileTimer -= 0.15f; 

    // for (int y {0}; y < dungeon->GetSize().y; ++y) {
    //     for (int x {0}; x < dungeon->GetSize().x; ++x) {
    //         DungeonNode& node {dungeon->GetNode(x, y)};
    //         switch (node.type) {
    //             case NodeType::Ground: {
    //                 auto tile {FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(x, y)};
    //                 int light {static_cast<int>((Random::Next() / 2.f + 0.5f) * 255.0f)};
    //                 tile->color = Color(static_cast<uint8_t>(light), static_cast<uint8_t>(light), static_cast<uint8_t>(light));
    //                 break;
    //             }
    //             case NodeType::Wall: {
    //                 auto tile {FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(x, y)};
    //                 int light {static_cast<int>((Random::Next() / 2.f + 0.5f) * 255.0f)};
    //                 tile->color = Color(static_cast<uint8_t>(light), static_cast<uint8_t>(light), static_cast<uint8_t>(light));
    //                 break;
    //             }
    //         }
    //     }
    // }

    //+ FOV Test:

    // // for (int y{dungeon->fov.minAffectedTile.y}; y < dungeon->fov.maxAffectedTile.y; ++y) {
    // //     for (int x{dungeon->fov.minAffectedTile.x}; x < dungeon->fov.maxAffectedTile.x; ++x) {
    // //         FovNode& fNode{dungeon->fov.GetNode(x, y)};
    // //         fNode.visible = false;
    // //         if (fNode.revealed)
    // //             fNode.lightLevel = REVEALED_LIGHT_LEVEL;
    // //         else
    // //             fNode.lightLevel = 0.0f;
    // //     }
    // // }

    // for (int y {0}; y < dungeon->fov.GetSize().y; ++y) {
    //     for (int x {0}; x < dungeon->fov.GetSize().x; ++x) {
    //         FovNode& fNode {dungeon->fov.GetNode(x, y)};
    //         fNode.visible = false;
    //         if (fNode.revealed)
    //             fNode.lightLevel = REVEALED_LIGHT_LEVEL;
    //         else    
    //             fNode.lightLevel = 0.0f;
    //     }
    // }

    // dungeon->fov.Compute(GetComponent<UnitComponent>().GetPosition(), 10, 2, FovType::Permissive);

    // for (int y {0}; y < dungeon->fov.GetSize().y; ++y) {
    //     for (int x {0}; x < dungeon->fov.GetSize().x; ++x) {
    //         DungeonNode& node {dungeon->GetNode(x, y)};
    //         FovNode& fNode {dungeon->fov.GetNode(x, y)};
    //         switch (node.type) {
    //             case NodeType::Ground: {
    //                 if (fNode.visible) {
    //                     auto tile{FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(x, y)};
    //                     // float light {glm::clamp(1.0f - (fNode.lightLevel / 6.0f) / 2.0f, 0.0f, 1.0f)};
    //                     float light {fNode.lightLevel};
    //                     tile->color = Color{light, light, light};
    //                 }
    //                 else {
    //                     if (fNode.revealed) {
    //                         auto tile{FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(x, y)};
    //                         float light {REVEALED_LIGHT_LEVEL};
    //                         tile->color = Color(light, light, light);
    //                         // tile->color = Color(1.0f, 0.0f, 0.0f);
    //                     }
    //                     else {
    //                         auto tile{FindGameObject(floorE)->GetComponent<Tilemap>().GetTile(x, y)};
    //                         tile->color = ColorNames::black;
    //                     }
    //                 }
    //                 break;
    //             }
    //             case NodeType::Wall: {
    //                 if (fNode.visible) {
    //                     auto tile{FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(x, y)};
    //                     // float light {glm::clamp(1.0f - (fNode.lightLevel / 6.0f) / 2.0f, 0.0f, 1.0f)};
    //                     float light {fNode.lightLevel};
    //                     tile->color = Color{light, light, light};
    //                 } else {
    //                     if (fNode.revealed) {
    //                         auto tile{FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(x, y)};
    //                         float light {REVEALED_LIGHT_LEVEL};
    //                         tile->color = Color(light, light, light);
    //                         // tile->color = Color(1.0f, 0.0f, 0.0f);
    //                     }
    //                     else {
    //                         auto tile{FindGameObject(wallsE)->GetComponent<Tilemap>().GetTile(x, y)};
    //                         tile->color = ColorNames::black;
    //                     }
    //                 }
    //                 break;
    //             }
    //         }
    //     }
    // }
}