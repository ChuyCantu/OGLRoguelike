#include "Level.hpp"

#include "Game/Player/Player.hpp"
#include "Game/TurnManager.hpp"
#include "Core/Tilemaps/Tilemap.hpp"

Level::Level(Engine* engine) : Scene{engine} {
    Load();
}

Level::~Level() {
    TurnManager::Instance().Clear();
}

void Level::Load() {
    // Level creation:
    auto tilemap {AddGameObject<GameObject>()}; 
    tilemap->name = "Dungeon Tilemap";
    auto& tmComp {tilemap->AddCommponent<Tilemap>()};
    tmComp.animationsDuration = 0.5f;
    tmComp.chunksSize = 24;
    tilemap->AddCommponent<TilemapCollider>().enabled = false;

    dungeon.CreateNew({40, 40}, 5, 10, {5, 4}, {10, 6});

    // for (int y{0}; y < dungeon.GetSize().y; ++y) {
    //     for (int x{0}; x < dungeon.GetSize().x; ++x) {
    //         if (x == 0 || y == 0 || x == dungeon.GetSize().x - 1 || y == dungeon.GetSize().y - 1)
    //             tileBrush.Paint(x, y, "tile0", tmComp);

    //         DungeonNode& node{dungeon.GetNode(x, y)};
    //         if (node.type == NodeType::Air)
    //             tileBrush.Paint(x, y, "tile3", tmComp);
    //         if (node.type == NodeType::Wall)
    //             tileBrush.Paint(x, y, "tile0", tmComp);
    //         else if (node.type == NodeType::Ground)
    //             tileBrush.Paint(x, y, "autotile0", tmComp);
    //         else if (node.type == NodeType::NodeTypeCount)
    //             tileBrush.Paint(x, y, "tile1", tmComp);
    //     }
    // }

    auto player {MakeOwned<Player>(this)};
    AddGameObject(std::move(player));
}

void Level::LastUpdate() {
    TurnManager::Instance().Update();
}

void Level::DebugGUI() {
    
}