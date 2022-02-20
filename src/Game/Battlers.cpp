#include "Battlers.hpp"

#include "Action.hpp"
#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Core/Time.hpp"
#include "Input/Input.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"

#include "UI/Text/TextRenderer.hpp"
#include "Utils/Color.hpp"
#include "UI/Label.hpp"
#include "UI/UI.hpp"
#include "UI/Slider.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

//+ BattlerPlayer =============================================
BattlerPlayer::BattlerPlayer(Scene* scene, const std::string& name) : Battler{scene, name} {
    tag = "Player";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 0}, glm::ivec2{16, 16}), ColorNames::white, 10)};
    // sr.sprite->flipX = true;
    // sr.pivot = glm::vec2{0.5f, 0.5f};

    auto& transform{GetComponent<Transform>()};
    transform.SetPosition(glm::vec2{0, 0});

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});

    AddCommponent<Collider>();

    AddCommponent<MoveComponent>().Teleport(glm::vec3{-32.f, 16.f, 0.0f});

    AddCommponent<BattlerComponent>(1, 10, 0, 100);

    transform.SetScale(glm::vec3{1.f, 1.f, 1.f});
}

BattlerPlayer::~BattlerPlayer() {

}

void BattlerPlayer::Update() {  
    auto& transform{GetComponent<Transform>()};
    // if (!GetComponent<BattlerComponent>().GetAction() && TurnManager::GetInstance().GetCurrentBattler() && *TurnManager::GetInstance().GetCurrentBattler() == *this) {
    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        if (Input::GetKey(SDL_SCANCODE_UP)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::up * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_DOWN)) {
            // GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + glm::vec3{1.0f, 1.0f, 0.0f} * 16.f, .15f));
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::down * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_LEFT)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::left * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_RIGHT)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::right * 16.f, .15f));
        }

        if (Input::GetKeyDown(SDL_SCANCODE_U)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<SkipAction>(this));
        }
    }

    Camera::GetMainCamera().SetPosition(transform.GetPosition());
}

void BattlerPlayer::OnCollisionEnter(const Collider& other) {
    Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());

    LOG_TRACE("Enter: {}", other.gameobject->name);
}

void BattlerPlayer::OnCollisionStay(const Collider& other) {
    LOG_TRACE("Stay: {}", other.gameobject->name);
}

void BattlerPlayer::OnCollisionExit(const Collider& other) {
    LOG_TRACE("Exit: {}", other.gameobject->name);
}

void BattlerPlayer::DebugGUI() {
    
}

//+ BattlerEnemy =============================================
BattlerEnemy::BattlerEnemy(Scene* scene, const std::string& name) : Battler{scene, name} {
    tag = "Enemy";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{48, 112}, glm::ivec2{16, 16}), ColorNames::white, 10)};
    auto& transform{GetComponent<Transform>()};
    transform.SetPosition(glm::vec3{16.f * 3, 0.0f, 0.0f});

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});

    AddCommponent<Collider>();

    AddCommponent<MoveComponent>().Teleport(transform.GetPosition());

    AddCommponent<BattlerComponent>(1, 10, 0, 100);
}

BattlerEnemy::~BattlerEnemy() {

}

void BattlerEnemy::Update() {
    // if (!GetComponent<BattlerComponent>().GetAction() && TurnManager::GetInstance().GetCurrentBattler() && *TurnManager::GetInstance().GetCurrentBattler() == *this) {
    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        auto& transform{GetComponent<Transform>()};
        if (Input::GetKey(SDL_SCANCODE_UP)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::up * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_DOWN)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::down * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_LEFT)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::left * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_RIGHT)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::right * 16.f, .15f));
        }

        if (Input::GetKeyDown(SDL_SCANCODE_U)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<SkipAction>(this));
        }
    }
}