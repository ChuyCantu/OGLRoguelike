#include "Battlers.hpp"

#include "Action.hpp"
#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Core/Time.hpp"
#include "Input/Input.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"

//+ BattlerPlayer =============================================
BattlerPlayer::BattlerPlayer(Scene* scene, const std::string& name) : Battler{scene, name} {
    tag = "Player";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 224}, glm::ivec2{16, 16}), glm::vec4{1.0f}, 10)};
    sr.flip = true;

    auto& transform{GetComponent<Transform>()};
    transform.SetPosition(glm::vec3{0, 0, 0});

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});

    AddCommponent<Collider>();

    AddCommponent<MoveComponent>().Teleport(transform.GetPosition());

    AddCommponent<BattlerComponent>(1, 10, 0, 100);
}

BattlerPlayer::~BattlerPlayer() {

}

void BattlerPlayer::Update() {  
    auto& transform{GetComponent<Transform>()};
    if (!GetComponent<BattlerComponent>().GetAction() && TurnManager::GetInstance().GetCurrentBattler() && *TurnManager::GetInstance().GetCurrentBattler() == *this) {
        
        if (Input::GetKeyDown(SDL_SCANCODE_W)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::up * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_S)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::down * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_A)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::left * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_D)) {
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
}

//+ BattlerEnemy =============================================
BattlerEnemy::BattlerEnemy(Scene* scene, const std::string& name) : Battler{scene, name} {
    tag = "Enemy";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{48, 112}, glm::ivec2{16, 16}), glm::vec4{1.0f}, 10)};
    auto& transform{GetComponent<Transform>()};
    transform.SetPosition(glm::vec3{16.f * 6, 16.f * 6, 0.0f});

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
    if (!GetComponent<BattlerComponent>().GetAction() && TurnManager::GetInstance().GetCurrentBattler() && *TurnManager::GetInstance().GetCurrentBattler() == *this) {
        auto& transform{GetComponent<Transform>()};
        if (Input::GetKeyDown(SDL_SCANCODE_W)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::up * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_S)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::down * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_A)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::left * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_D)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::right * 16.f, .15f));
        }

        if (Input::GetKeyDown(SDL_SCANCODE_U)) {
            GetComponent<BattlerComponent>().SetAction(MakeOwned<SkipAction>(this));
        }
    }
}