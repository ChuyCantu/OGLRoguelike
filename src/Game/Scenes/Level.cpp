#include "Level.hpp"

#include "Game/AutotilesLoaders.hpp"
#include "Game/Player/Player.hpp"
#include "Game/TurnManager.hpp"
#include "Core/Tilemaps/Tilemap.hpp"
#include "Utils/Random.hpp"

Level::Level(Engine* engine) : Scene{engine} {
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

    dungeon.CreateNew({40, 40}, 5, 10, {5, 4}, {10, 6});

    auto floorBrush {AssetManager::GetTileBrush("floor")};
    floorBrush->GetTileAsset(0)->layer = -1;
    wallBrush->GetTileAsset(48)->layer = 0;
    floorBrush->GetTileAsset(2)->layer = 1;

    for (int y{0}; y < dungeon.GetSize().y; ++y) {
        for (int x{0}; x < dungeon.GetSize().x; ++x) {
            // temp border:
            if (x == 0 || y == 0 || x == dungeon.GetSize().x - 1 || y == dungeon.GetSize().y - 1)
                floorBrush->Paint(x, y, 0, floorTmComp);

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
    const Rect& room {dungeon.GetRooms()[Random::Range(0, dungeon.GetRooms().size() - 1)]};
    int playerX {Random::Range(static_cast<int>(room.position.x), static_cast<int>(room.position.x + room.size.x) - 1)};
    int playerY {Random::Range(static_cast<int>(room.position.y), static_cast<int>(room.position.y + room.size.y) - 1)};
    player->GetComponent<MoveComponent>().Teleport(glm::vec3{playerX * 16, playerY * 16, 0});
    LOG_TRACE("Player starting position: ({}, {})", playerX, playerY);
    AddGameObject(std::move(player));
}

void Level::LastUpdate() {
    TurnManager::Instance().Update();
}

void Level::DebugGUI() {
    
}