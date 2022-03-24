#include "Level.hpp"

#include "Game/AutotilesLoaders.hpp"
#include "Game/Player/Player.hpp"
#include "Game/TurnManager.hpp"
#include "Core/Tilemaps/Tilemap.hpp"

#include "Rendering/Sprite.hpp"

Level::Level(Engine* engine) : Scene{engine} {
    Load();
}

Level::~Level() {
    TurnManager::Instance().Clear();
}

void Level::Load() {
    Ref<TileBrush> wallBrush {MakeRef<TileBrush>()};
    AssetManager::AddTileBrush("wall", wallBrush);
    std::unordered_map<int, TileRule> rules;
    CreateWallRules(AssetManager::GetTexture("wall_spritesheet"), {112, 288}, 16, rules);
    wallBrush->CreateAutoTile("template", rules[0].sprites[0], rules);

    // Level creation:
    auto tilemap {AddGameObject<GameObject>()}; 
    tilemap->name = "Dungeon Tilemap";
    auto& tmComp {tilemap->AddCommponent<Tilemap>()};
    tmComp.animationsDuration = 0.5f;
    tmComp.chunksSize = 24;
    tilemap->AddCommponent<TilemapCollider>().enabled = false;

    dungeon.CreateNew({40, 40}, 5, 10, {5, 4}, {10, 6});

    auto floorBrush {AssetManager::GetTileBrush("floor")};
    floorBrush->GetTileAsset("template")->layer = -1;
    wallBrush->GetTileAsset("template")->layer = 0;
    floorBrush->GetTileAsset("02")->layer = 1;

    for (int y{0}; y < dungeon.GetSize().y; ++y) {
        for (int x{0}; x < dungeon.GetSize().x; ++x) {
            if (x == 0 || y == 0 || x == dungeon.GetSize().x - 1 || y == dungeon.GetSize().y - 1)
                floorBrush->Paint(x, y, "template", tmComp);

            DungeonNode& node{dungeon.GetNode(x, y)};
            // if (node.type == NodeType::Air)
            //     floorBrush->Paint(x, y, "tile3", tmComp);
            if (node.type == NodeType::Wall)
                wallBrush->Paint(x, y, "template", tmComp);
            else if (node.type == NodeType::Ground)
                floorBrush->Paint(x, y, "02", tmComp);
        }
    }

    auto player {MakeOwned<Player>(this)};
    AddGameObject(std::move(player));
}

void Level::LastUpdate() {
    TurnManager::Instance().Update();
}

void Level::DebugGUI() {
    
}