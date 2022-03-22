#include "Player.hpp"

#include "Game/Action.hpp"
#include "Input/Input.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"

#include <imgui.h>

Player::Player(Scene* scene, const std::string& name) : Unit{scene, name} {
    tag = "Player";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 0}, glm::ivec2{16, 16}), ColorNames::white, 10)};

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});

    AddCommponent<Collider>();

    AddCommponent<MoveComponent>().Teleport(glm::vec3{0.f, 0.f, 0.0f});

    AddCommponent<UnitComponent>(1, 10, 0, 100);
}

Player::~Player() {
    
}

void Player::Update() {
    stateMachine.Update();

    auto& transform{GetComponent<Transform>()};
    
    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        if (Input::GetKey(SDL_SCANCODE_UP)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::up * 16.f, .15f));
        }
        if (Input::GetKey(SDL_SCANCODE_DOWN)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::down * 16.f, .15f));
        }
        if (Input::GetKey(SDL_SCANCODE_LEFT)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::left * 16.f, .15f));
        }
        if (Input::GetKey(SDL_SCANCODE_RIGHT)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::right * 16.f, .15f));
        }

        if (Input::GetKeyDown(SDL_SCANCODE_U)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
        }
    }

    Camera::GetMainCamera().SetPosition(transform.GetPosition());
}

void Player::OnCollisionEnter(const Collider& other) {
    Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());
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
    ImGui::PopStyleColor();
    ImGui::End();
}