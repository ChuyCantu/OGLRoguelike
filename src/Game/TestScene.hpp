#ifndef __TESTSCENE_H__
#define __TESTSCENE_H__

#include "Core/AssetManager.hpp"
#include "Core/Scene.hpp"

class TestScene : public Scene {
public:
    TestScene(class Engine* engine);
    ~TestScene() override;

    void Load();
    void LastUpdate() override;
};

#endif // __TESTSCENE_H__