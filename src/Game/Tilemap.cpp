#include "Tilemap.hpp"

#include "Core/AssetsManager.hpp"
#include "Core/Log.hpp"
#include "Input/Input.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Texture.hpp"
#include "Utils/Random.hpp"

Tilemap::Tilemap(Scene* scene) : GameObject{scene, "Tilemap"} {
    auto& tilemap {AddCommponent<TilemapRenderer>()};
    tilemap.Construct(glm::ivec2{10, 10}, 16, AssetsManager::GetTexture("Pit0"));

    auto& animator {AddCommponent<Animator>()};
    animator.frames.push_back(Animator::Frame{AssetsManager::GetTexture("pit0_spritesheet"), 0.5f});
    animator.frames.push_back(Animator::Frame{AssetsManager::GetTexture("pit1_spritesheet"), 0.5f});

    tilemap.SetTile(0, 0, 17);
    tilemap.SetTile(1, 0, 18);
    tilemap.SetTile(2, 0, 19);

    // tilemap.SetTile(0, 1, 18);
    // tilemap.SetTile(1, 1, 18);
    tilemap.SetTile(2, 1, 18);
    tilemap.SetTile(3, 1, 18);
    tilemap.SetTile(4, 1, 18);
    tilemap.SetTile(5, 1, 18);
    tilemap.SetTile(6, 1, 18);
    tilemap.SetTile(7, 1, 18);
    tilemap.SetTile(8, 1, 18);
    tilemap.SetTile(9, 1, 18);

    tilemap.SetTile(0, 2, 18);
    tilemap.SetTile(1, 2, 18);

    tilemap.SetTile(8, 8, 18);

    // for (int y {0}; y < 10; ++y) {
    //     for (int x {0}; x < 10; ++x) {
    //         tilemap.SetTile(x, y, 18);
    //     }
    // }

    // Check why only first y's work
    
}

Tilemap::~Tilemap() {
    
}

void Tilemap::Start() {
    
}

void Tilemap::Update() {
    if (Input::GetMouseButtonDown(SDL_BUTTON_LEFT)) {
        auto& tilemap {GetComponent<TilemapRenderer>()};

        auto clickPos {Camera::GetMainCamera().ScreenToWorld2D(Input::GetMousePosition())};
        auto tilemapClickPos {glm::ivec2{static_cast<int>(clickPos.x / tilemap.GetTileSize()), static_cast<int>(clickPos.y / tilemap.GetTileSize())}};
        LOG_TRACE("Click: {}, {}", tilemapClickPos.x, tilemapClickPos.y);

        //! This without fixing the position since the tilemap is in the origin of the world for this example
        if ((clickPos.x >= 0 && clickPos.y >= 0) && tilemapClickPos.x >= 0 && tilemapClickPos.x < tilemap.GetSize().x && tilemapClickPos.y >= 0 && tilemapClickPos.y < tilemap.GetSize().y) {
            tilemap.SetTile(tilemapClickPos.x, tilemapClickPos.y, Random::Range(0, tilemap.GetAtlasTexSize().x * tilemap.GetAtlasTexSize().y));
        }
    }
}

void Tilemap::OnEnable() {
    
}

void Tilemap::OnDisable() {
    
}

void Tilemap::OnDestroy() {
    
}
