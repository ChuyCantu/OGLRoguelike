#pragma once

#include "Core/AssetManager.hpp"
#include "Core/Scene.hpp"

class TestScene2 : public Scene {
public:
    TestScene2(class Engine* engine);
    ~TestScene2() override;

    void Load();
    void LastUpdate() override;

    void DebugGUI() override;
};