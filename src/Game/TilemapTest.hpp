#ifndef __TILEMAPTEST_H__
#define __TILEMAPTEST_H__

#include "Core/GameObject.hpp"

class TilemapTest : public GameObject {
public:
    TilemapTest(Scene* scene);
    ~TilemapTest() override;

    void Start() override;
    void Update() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnDestroy() override;

private:
    GameObject* groundTM;
    GameObject* wallsTM;
};

#endif  // __TILEMAPTEST_H__