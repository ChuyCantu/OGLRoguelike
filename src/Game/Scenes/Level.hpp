#pragma once

#include "Core/Scene.hpp"
#include "Game/DungeonGen/Dungeon.hpp"

class Level : public Scene {
public:
    Level(class Engine* engine);
    ~Level() override;

    void Load();
    void LastUpdate() override;

    void DebugGUI() override;

public:
    Dungeon dungeon;
};