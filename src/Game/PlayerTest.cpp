#include "PlayerTest.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Core/Time.hpp"
#include "Input/Input.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Sprite.hpp"

#include "Core/Log.hpp"

PlayerTest::PlayerTest(Scene* scene) : GameObject{scene, "Player"} {
    tag = "Player";
    auto& sr {AddCommponent<SpriteRenderer>(MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 224}, glm::ivec2{16, 16}), glm::vec4{1.0f}, 10)};
    sr.flip = true;
    // sr.sprite = MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 224}, glm::ivec2{16, 16});
    // sr.renderOrder = 10;
    auto& transform {GetComponent<Transform>()};
    transform.SetPosition(glm::vec3{16.f, 16.f, 0.0f});
    // RotateAroundPivot(transform, transform.GetPosition() + glm::vec3{8.f, 8.f, 0.f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::radians(30.f));

    auto& animator {AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});

    AddCommponent<Collider>();
}

PlayerTest::PlayerTest(Scene* scene, float scale) : GameObject{scene, "Player"} {
    auto& sr{AddCommponent<SpriteRenderer>()};
    sr.sprite = MakeRef<Sprite>(AssetManager::GetTexture("player0_spritesheet"), glm::ivec2{64, 224}, glm::ivec2{16, 16});
    sr.renderOrder = 10;
    auto& transform{GetComponent<Transform>()};
    transform.SetPosition(glm::vec3{-32.f, -32.f, 0.0f});
    // RotateAroundPivot(transform, transform.GetPosition() + glm::vec3{8.f, 8.f, 0.f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::radians(30.f));
    transform.SetScale(glm::vec3{-1.f, 1.f, 1.f} * scale);

    auto& animator{AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("player1_spritesheet"), 0.5f});
}

PlayerTest::~PlayerTest() {
    
}

void PlayerTest::Start() {
    LOG_TRACE("Player Started.");
}

void PlayerTest::Update() {
    auto& transform {GetComponent<Transform>()};

    // float speed {50.f};
    // if (Input::GetKey(SDL_SCANCODE_W)) {
    //     transform.SetPosition(transform.GetPosition() + glm::vec3{0.0f, speed, 0.0f} * Time::deltaTime);
    // }
    // if (Input::GetKey(SDL_SCANCODE_S)) {
    //     transform.SetPosition(transform.GetPosition() + glm::vec3{0.0f, -speed, 0.0f} * Time::deltaTime);
    // }
    // if (Input::GetKey(SDL_SCANCODE_A)) {
    //     transform.SetPosition(transform.GetPosition() + glm::vec3{-speed, 0.0f, 0.0f} * Time::deltaTime);
    // }
    // if (Input::GetKey(SDL_SCANCODE_D)) {
    //     transform.SetPosition(transform.GetPosition() + glm::vec3{speed, 0.0f, 0.0f} * Time::deltaTime);
    // }

    float speed {16.0f};
    if (Input::GetKeyDown(SDL_SCANCODE_W)) {
        transform.SetPosition(transform.GetPosition() + glm::vec3{0.0f, speed, 0.0f});
    }
    if (Input::GetKeyDown(SDL_SCANCODE_S)) {
        transform.SetPosition(transform.GetPosition() + glm::vec3{0.0f, -speed, 0.0f});
    }
    if (Input::GetKeyDown(SDL_SCANCODE_A)) {
        transform.SetPosition(transform.GetPosition() + glm::vec3{-speed, 0.0f, 0.0f});
    }
    if (Input::GetKeyDown(SDL_SCANCODE_D)) {
        transform.SetPosition(transform.GetPosition() + glm::vec3{speed, 0.0f, 0.0f});
    }

    if (Input::GetKeyDown(SDL_SCANCODE_P))
        Destroy();

    if (Input::GetKeyDown(SDL_SCANCODE_R)) {
        for (auto&& [entity, transform, anim] : scene->ViewComponents<Transform, Tilemap<Tile>>().each()) {
            LOG_TRACE("Entity with Tilemap<Tile>: {} [{}]", entt::to_integral(entity), transform.gameobject->tag);
        }
    }

    Camera::GetMainCamera().SetPosition(transform.GetPosition());
}

void PlayerTest::OnEnable() {
    
}

void PlayerTest::OnDisable() {
    
}

void PlayerTest::OnDestroy() {
    
}

// void PlayerTest::OnCollision(const Collider& other) {
//     // Fix camera position in case of a collision
//     Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());
//     LOG_TRACE("Player collided with a {}", other.gameobject->tag);
// }

void PlayerTest::OnCollisionEnter(const Collider& other) {
    Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());
    LOG_TRACE("Player collided (enter) with a {}", other.gameobject->tag);
}

void PlayerTest::OnCollisionStay(const Collider& other) {
    // Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());
    // LOG_TRACE("Player collided (stay) with a {}", other.gameobject->tag);
}

void PlayerTest::OnCollisionExit(const Collider& other) {
    // Camera::GetMainCamera().SetPosition(GetComponent<Transform>().GetPosition());
    LOG_TRACE("Player collided (exit) with a {}", other.gameobject->tag);
}
