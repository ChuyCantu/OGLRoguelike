#include "Battlers.hpp"

#include "Game/Action.hpp"
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

class TestState1 : public State {
public:
    TestState1() : State{"TestState1"} { }

    void OnEnter() override {
        LOG_TRACE("Hi from {}!", name);
    }

    void OnExit() override {
        LOG_TRACE("Good bye from {}!", name);
    }
};

class TestState2 : public State {
public:
    TestState2() : State{"TestState2"} {}

    void OnEnter() override {
        LOG_TRACE("Hi from {}!", name);
    }

    void OnExit() override {
        LOG_TRACE("Good bye from {}!", name);
    }
};

//+ UnitPlayer =============================================
UnitPlayer::UnitPlayer(Scene* scene, const std::string& name) : Unit{scene, name} {
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

    AddCommponent<UnitComponent>(1, 10, 0, 100);

    transform.SetScale(glm::vec3{1.f, 1.f, 1.f});

    // StateMachine:
    auto state1 {MakeRef<TestState1>()};
    auto state2 {MakeRef<TestState2>()};

    stateMachine.AddState(state1);
    stateMachine.AddState(state2);

    stateMachine.AddAnyTransition(state1, [this]() { return Input::GetKeyDown(SDL_SCANCODE_P); });
    stateMachine.AddAnyTransition(state2, []() { return Input::GetKeyDown(SDL_SCANCODE_O); });

    stateMachine.SetState(state1);
}

UnitPlayer::~UnitPlayer() {

}

void UnitPlayer::Update() {  
    stateMachine.Update();

    auto& transform{GetComponent<Transform>()};
    // if (!GetComponent<UnitComponent>().GetAction() && TurnManager::GetInstance().GetCurrentUnit() && *TurnManager::GetInstance().GetCurrentUnit() == *this) {
    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        if (Input::GetKey(SDL_SCANCODE_UP)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::up * 16.f, .15f));
        }
        if (Input::GetKey(SDL_SCANCODE_DOWN)) {
            // GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + glm::vec3{1.0f, 1.0f, 0.0f} * 16.f, .15f));
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

void UnitPlayer::OnCollisionEnter(const Collider& other) {
    Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());

    // LOG_TRACE("Enter: {}", other.gameobject->name);
}

void UnitPlayer::OnCollisionStay(const Collider& other) {
    // LOG_TRACE("Stay: {}", other.gameobject->name);
}

void UnitPlayer::OnCollisionExit(const Collider& other) {
    // LOG_TRACE("Exit: {}", other.gameobject->name);
}

void UnitPlayer::DebugGUI() {
    
}

//+ UnitEnemy =============================================
UnitEnemy::UnitEnemy(Scene* scene, const std::string& name) : Unit{scene, name} {
    tag = "Enemy";
    auto& sr{AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{48, 112}, glm::ivec2{16, 16}), ColorNames::white, 10)};
    auto& transform{GetComponent<Transform>()};
    transform.SetPosition(glm::vec3{16.f * 3, 0.0f, 0.0f});

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});

    AddCommponent<Collider>();

    AddCommponent<MoveComponent>().Teleport(transform.GetPosition());

    AddCommponent<UnitComponent>(1, 10, 0, 100);
}

UnitEnemy::~UnitEnemy() {

}

void UnitEnemy::Update() {
    // if (!GetComponent<UnitComponent>().GetAction() && TurnManager::GetInstance().GetCurrentUnit() && *TurnManager::GetInstance().GetCurrentUnit() == *this) {
    if (TurnManager::Instance().CanPerformNewAction(*this)) {
        auto& transform{GetComponent<Transform>()};
        if (Input::GetKey(SDL_SCANCODE_UP)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::up * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_DOWN)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::down * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_LEFT)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::left * 16.f, .15f));
        }
        if (Input::GetKeyDown(SDL_SCANCODE_RIGHT)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<MoveAction>(this, transform.GetPosition() + vec3::right * 16.f, .15f));
        }

        if (Input::GetKeyDown(SDL_SCANCODE_U)) {
            GetComponent<UnitComponent>().SetAction(MakeOwned<SkipAction>(this));
        }
    }
}