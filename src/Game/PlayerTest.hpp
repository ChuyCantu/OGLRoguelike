#ifndef __PLAYERTEST_H__
#define __PLAYERTEST_H__

#include "Core/GameObject.hpp"

class PlayerTest : public GameObject {
public:
    PlayerTest(Scene* scene);
    PlayerTest(Scene* scene, float scale);
    ~PlayerTest() override;

    void Start() override;
    void Update() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnDestroy() override;

    void OnCollision(const Collider& other) override;
    void OnCollisionEnter(const Collider& other) override;
    void OnCollisionStay(const Collider& other) override;
    void OnCollisionExit(const Collider& other) override;
};

#endif // __PLAYERTEST_H__