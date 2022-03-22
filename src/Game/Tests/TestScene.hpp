#pragma once

#include "Core/AssetManager.hpp"
#include "Core/Scene.hpp"

class TestScene : public Scene {
public:
    TestScene(class Engine* engine);
    ~TestScene() override;

    void Load();
    void LastUpdate() override;

    void DebugGUI() override;
};