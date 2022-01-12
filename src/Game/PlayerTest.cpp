#include "PlayerTest.hpp"

#include "Core/AssetsManager.hpp"
#include "Core/Components.hpp"
#include "Core/Time.hpp"
#include "Input/Input.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Sprite.hpp"

PlayerTest::PlayerTest(Scene* scene) : GameObject{scene, "Player"} {
    auto& sr {AddCommponent<SpriteRenderer>()};
    // sr.sprite = MakeRef<Sprite>(AssetsManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 224}, glm::ivec2{16, 16});
    sr.renderOrder = 10;
    auto& transform {GetComponent<Transform>()};
    transform.SetPosition(glm::vec3{-32.f, -32.f, 0.0f});
    RotateAroundPivot(transform, transform.GetPosition() + glm::vec3{8.f, 8.f, 0.f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::radians(30.f));
}

PlayerTest::~PlayerTest() {
    
}

void PlayerTest::Start() {
    
}

void PlayerTest::Update() {
    // auto& input { Input::system->GetState().Keyboard };
    float speed {50.f};

    if (Input::GetKey(SDL_SCANCODE_W)/*input.GetKeyValue(SDL_SCANCODE_W)*/) {
        auto& transform {GetComponent<Transform>()};
        transform.SetPosition(transform.GetPosition() + glm::vec3{0.0f, speed, 0.0f} * Time::deltaTime);
    }
    if (Input::GetKey(SDL_SCANCODE_S)/*input.GetKeyValue(SDL_SCANCODE_S)*/) {
        auto& transform{GetComponent<Transform>()};
        transform.SetPosition(transform.GetPosition() + glm::vec3{0.0f, -speed, 0.0f} * Time::deltaTime);
    }
    if (Input::GetKey(SDL_SCANCODE_A)/*input.GetKeyValue(SDL_SCANCODE_A)*/) {
        auto& transform{GetComponent<Transform>()};
        transform.SetPosition(transform.GetPosition() + glm::vec3{-speed, 0.0f, 0.0f} * Time::deltaTime);
    }
    if (Input::GetKey(SDL_SCANCODE_D)/*input.GetKeyValue(SDL_SCANCODE_D)*/) {
        auto& transform{GetComponent<Transform>()};
        transform.SetPosition(transform.GetPosition() + glm::vec3{speed, 0.0f, 0.0f} * Time::deltaTime);
    }
    if (Input::GetKeyDown(SDL_SCANCODE_P)/*input.GetKeyState(SDL_SCANCODE_P) == ButtonState::Pressed*/)
        Destroy();
}

void PlayerTest::OnEnable() {
    
}

void PlayerTest::OnDisable() {
    
}

void PlayerTest::OnDestroy() {
    
}
