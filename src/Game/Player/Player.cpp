#include "Player.hpp"

#include "Game/Constants.hpp"
#include "Game/Action.hpp"
#include "Game/DungeonGen/Dungeon.hpp"
#include "Input/Input.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"

#include <imgui.h>

Player::Player(Scene* scene, const std::string& name) : Unit{scene, name} {
    tag = "Player";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 0}, glm::ivec2{16, 16}), ColorNames::white, 0)};

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});

    AddCommponent<Collider>();

    // AddCommponent<MoveComponent>().Teleport(glm::vec3{0.f, 0.f, 0.0f});

    AddCommponent<UnitComponent>(1, 10, 0, 100);
}

void Player::Update() {
    stateMachine.Update();

    auto& transform{GetComponent<Transform>()};
    
    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        if (Input::GetKeyDown(SDL_SCANCODE_UP)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + vec3::up * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_DOWN)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + vec3::down * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_LEFT)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + vec3::left * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_RIGHT)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + vec3::right * TILE_SIZEF, .15f, dungeon));
        }

        if (Input::GetKeyDown(SDL_SCANCODE_U)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
        }

        if (Input::GetKeyDown(SDL_SCANCODE_KP_8)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + vec3::up * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_2)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + vec3::down * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_4)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + vec3::left * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_6)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + vec3::right * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_7)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + glm::vec3{-1.0f, 1.0f, 0.0f} * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_9)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + glm::vec3{1.0f, 1.0f, 0.0f} * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_3)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + glm::vec3{1.0f, -1.0f, 0.0f} * TILE_SIZEF, .15f, dungeon));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_KP_1)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveUnitAction>(this, transform.GetPosition() + glm::vec3{-1.0f, -1.0f, 0.0f} * TILE_SIZEF, .15f, dungeon));
        }
    }

    Camera::GetMainCamera().SetPosition(transform.GetPosition());

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

void Player::DebugGUI() {
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::SetNextWindowPos(ImVec2{2.f, 2.f}, 0, ImVec2{0.f, 0.f});
    ImGui::SetNextWindowSize(ImVec2{75, 85});
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
    DungeonNode* dnode {dungeon->TryGetNode(tiledPos.x, tiledPos.y)};
    if (dnode && dnode->unit)
        ImGui::Text("u?: true");
    else
        ImGui::Text("u?: false");
    //     cost = dungeon->pathfinding.GetNode(tiledPos).g;
    // ImGui::Text("c: %i", cost);
    ImGui::PopStyleColor();
    ImGui::End();
}