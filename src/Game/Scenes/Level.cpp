#include "Level.hpp"

#include "Core/Engine.hpp"
#include "Core/Tilemaps/Tilemap.hpp"
#include "Game/AutotilesLoaders.hpp"
#include "Game/Enemies/Enemy.hpp"
#include "Game/Player/Player.hpp"
#include "Game/TurnManager.hpp"
#include "Rendering/Renderer.hpp"
#include "Utils/Random.hpp"

#include <imgui.h>

Level::Level(Engine* engine) : Scene{engine} {
    // engine->GetRenderer()->SetClearColor(0.0823f, 0.0509f, 0.1098f);

    Load();
}

Level::~Level() {
    TurnManager::Instance().Clear();
}

void Level::Load() {
    auto wallBrush {AssetManager::GetTileBrush("wall")};

    // Level creation:
    auto dungeonTilemaps {AddGameObject<GameObject>()}; 
    dungeonTilemaps->name = "Dungeon Tilemaps";
    auto& dungeonTilemapsTransform {dungeonTilemaps->GetComponent<Transform>()};

    auto floorTilemap {AddGameObject<GameObject>()};
    dungeonTilemapsTransform.AddChild(floorTilemap);
    auto& floorTmComp {floorTilemap->AddCommponent<Tilemap>()};
    floorTmComp.animationsDuration = 0.5f;
    floorTmComp.chunksSize = 24;

    auto wallTilemap {AddGameObject<GameObject>()};
    dungeonTilemapsTransform.AddChild(wallTilemap);
    auto& wallTmComp {wallTilemap->AddCommponent<Tilemap>()};
    wallTmComp.animationsDuration = 0.5f;
    wallTmComp.chunksSize = 24;
    wallTilemap->AddCommponent<TilemapCollider>();

    // dungeon.CreateNew({40, 40}, 5, 10, {5, 4}, {10, 6});
    dungeon.CreateNew({20, 20}, 2, 3, {4, 4}, {6, 6});
    // dungeon.CreateNew({80, 80}, 2, 3, {30, 30}, {40, 30});

    auto floorBrush {AssetManager::GetTileBrush("floor")};
    floorBrush->GetTileAsset(0)->layer = -1;
    wallBrush->GetTileAsset(48)->layer = 0;
    floorBrush->GetTileAsset(2)->layer = 1;

    for (int y{0}; y < dungeon.GetSize().y; ++y) {
        for (int x{0}; x < dungeon.GetSize().x; ++x) {
            // // temp border:
            // if (x == 0 || y == 0 || x == dungeon.GetSize().x - 1 || y == dungeon.GetSize().y - 1)
            //     floorBrush->Paint(x, y, 0, floorTmComp);

            DungeonNode& node{dungeon.GetNode(x, y)};
            // if (node.type == NodeType::Air)
            //     floorBrush->Paint(x, y, "tile3", tmComp);
            if (node.type == NodeType::Wall)
                wallBrush->Paint(x, y, 48, wallTmComp);
            else if (node.type == NodeType::Ground)
                floorBrush->Paint(x, y, 2, floorTmComp);
        }
    }

    // Randomly place player in a room:
    auto player {MakeOwned<Player>(this)};
    entt::entity playerEntity {player->Entity()};
    player->dungeon = &dungeon;
    const Rect& room {dungeon.GetRooms()[Random::Range(0, dungeon.GetRooms().size() - 1)]};
    int playerX {Random::Range(static_cast<int>(room.position.x), static_cast<int>(room.position.x + room.size.x) - 1)};
    int playerY {Random::Range(static_cast<int>(room.position.y), static_cast<int>(room.position.y + room.size.y) - 1)};
    // player->GetComponent<MoveComponent>().Teleport(glm::vec3{playerX * 16, playerY * 16, 0});
    player->SetStartPosition(glm::ivec2{playerX, playerY});
    // LOG_TRACE("Player starting position: ({}, {})", playerX, playerY);
    AddGameObject(std::move(player));

    // Enemy test
    for (int i {0}; i < 3; ++i) {
        auto slime {MakeOwned<Enemy>(this)};
        slime->playerRef = playerEntity;
        slime->dungeon = &dungeon;
        const Rect& room2 {dungeon.GetRooms()[Random::Range(0, dungeon.GetRooms().size() - 1)]};
        int slimeX {Random::Range(static_cast<int>(room2.position.x), static_cast<int>(room2.position.x + room2.size.x) - 1)};
        int slimeY {Random::Range(static_cast<int>(room2.position.y), static_cast<int>(room2.position.y + room2.size.y) - 1)};
        // slime->GetComponent<MoveComponent>().Teleport(glm::vec3{slimeX * 16, slimeY * 16, 0});
        slime->SetStartPosition(glm::ivec2{slimeX, slimeY});
        // LOG_TRACE("Slime starting position: ({}, {})", slimeX, slimeY);
        AddGameObject(std::move(slime));
    }

    // Make the player have the first turn to prevent actions that need the players position to fail on the first turn
    TurnManager::Instance().SkipUnitsBeforePlayer();
}

void Level::LastUpdate() {
    TurnManager::Instance().Update();
}

void Level::DebugGUI() {

}