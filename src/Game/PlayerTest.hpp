#ifndef __PLAYERTEST_H__
#define __PLAYERTEST_H__

#include "Core/GameObject.hpp"

class PlayerTest : public GameObject {
public:
    PlayerTest(Scene* scene);
    ~PlayerTest() override;

    void Start() override;
    void Update() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnDestroy() override;
};

#endif // __PLAYERTEST_H__