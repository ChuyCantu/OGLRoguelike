#include "TilemapTest.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Input/Input.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Texture.hpp"
#include "Utils/Random.hpp"

TilemapTest::TilemapTest(Scene* scene) : GameObject{scene, "TilemapTest"} {
    // tag = "TilemapTest";
    // auto& tilemapR {AddCommponent<TilemapRenderer>(glm::ivec2{100, 100}, 16, AssetManager::GetTexture("pit0_spritesheet"), 0)};
    // // tilemapR.Construct(glm::ivec2{100, 100}, 16, AssetManager::GetTexture("pit0_spritesheet"));

    // auto& tilemap {AddCommponent<Tilemap<Tile>>(tilemapR.GetSize())};

    // AddCommponent<TilemapCollider>().canPassThrough = true;

    // auto& animator {AddCommponent<Animator>()};
    // animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("pit0_spritesheet"), 0.5f});
    // animator.frames.push_back(Animator::Frame{AssetManager::GetTexture("pit1_spritesheet"), 0.5f});

    // for (int y{0}; y < tilemapR.GetSize().y; ++y) {
    //     for (int x{0}; x < tilemapR.GetSize().x; ++x) {
    //         tilemapR.SetTile(x, y, 209);
    //         tilemap.GetTile(x, y).cost = Random::Range(5, 10);
    //     }
    // }

    // --------------------------------------------
    auto& tilemapSize {glm::ivec2{10, 10}};
    groundTM = scene->AddGameObject<GameObject>();
    groundTM->tag = "Ground";
    wallsTM = scene->AddGameObject<GameObject>();
    wallsTM->tag = "Walls";
    groundTM->AddCommponent<TilemapRendererOld>(tilemapSize, 16, AssetManager::GetTexture("pit0_spritesheet"), 0);
    wallsTM->AddCommponent<TilemapRendererOld>(tilemapSize, 16, AssetManager::GetTexture("pit0_spritesheet"), 1);
    groundTM->AddCommponent<TilemapOld<TileOld>>(tilemapSize);
    wallsTM->AddCommponent<TilemapOld<TileOld>>(tilemapSize);
    wallsTM->AddCommponent<TilemapCollider>().isSolid = true;

    for (int y {0}; y < tilemapSize.y; ++y) {
        for (int x {0}; x < tilemapSize.x; ++x) {
            groundTM->GetComponent<TilemapRendererOld>().SetTile(x, y, 34);
            groundTM->GetComponent<TilemapOld<TileOld>>().GetTile(x, y).cost = 1;

            if (x == tilemapSize.x - 1 || y == tilemapSize.y - 1 || x == 0 || y == 0)
                wallsTM->GetComponent<TilemapRendererOld>().SetTile(x, y, 18);
        }
    }

    GetComponent<Transform>().SetPosition(glm::vec3{16.f * 5, 16.f * 5, 0.f});
    wallsTM->GetComponent<Transform>().SetPosition(glm::vec3{16.f * 5, 16.f * 5, 0.f});
    groundTM->GetComponent<Transform>().SetPosition(glm::vec3{16.f * 5, 16.f * 5, 0.f});
}

TilemapTest::~TilemapTest() {
    // if (wallsTM)
    //     wallsTM->Destroy();
    // if (groundTM)
    //     groundTM->Destroy();
}

void TilemapTest::Start() {
    
}

void TilemapTest::Update() {
    // if (Input::GetMouseButtonDown(SDL_BUTTON_LEFT)) {
    //     auto& tilemapR {groundTM->GetComponent<TilemapRenderer>()};

    //     auto clickPos {Camera::GetMainCamera().ScreenToWorld2D(Input::GetMousePosition())};
    //     auto tilemapClickPos {glm::ivec2{static_cast<int>(clickPos.x / tilemapR.GetTileSize()), static_cast<int>(clickPos.y / tilemapR.GetTileSize())}};
    //     LOG_TRACE("Click: {}, {}", tilemapClickPos.x, tilemapClickPos.y);

    //     //! This without fixing the position since the tilemap is in the origin of the world for this example
    //     if ((clickPos.x >= 0 && clickPos.y >= 0) && tilemapClickPos.x >= 0 && tilemapClickPos.x < tilemapR.GetSize().x && tilemapClickPos.y >= 0 && tilemapClickPos.y < tilemapR.GetSize().y) {
    //         tilemapR.SetTile(tilemapClickPos.x, tilemapClickPos.y, Random::Range(0, tilemapR.GetAtlasTexSize().x * tilemapR.GetAtlasTexSize().y));
    //         LOG_TRACE("Cost ({}, {}) = {}", tilemapClickPos.x, tilemapClickPos.y, groundTM->GetComponent<Tilemap<Tile>>().GetTile(tilemapClickPos.x, tilemapClickPos.y).cost);
    //     }
    // }
}

void TilemapTest::OnEnable() {
    
}

void TilemapTest::OnDisable() {
    
}

void TilemapTest::OnDestroy() {
    
}
