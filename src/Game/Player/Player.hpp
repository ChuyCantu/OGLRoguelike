#pragma once

#include "Core/Event.hpp"
#include "Game/AI/StateMachine.hpp"
#include "Game/TurnManager.hpp"

class Player : public Unit {
public:
    Player(class Scene* scene, const std::string& name = "Player");
    
    void Update() override;
    void OnCollisionEnter(const Collider& other) override;
    void OnCollisionStay(const Collider& other) override;
    void OnCollisionExit(const Collider& other) override;
    void DebugGUI() override;

    void SetStartPosition(const glm::ivec2 position);

private:
    void MoveToTile(const glm::ivec2& dest, float duration);
    void MoveCamera();

private:
    StateMachine stateMachine;
    
    float cameraMoveTimer    {0.0f};
    float cameraMoveDuration {0.0f};
    glm::vec3 cameraSrcPos   {0.0f};
    glm::vec3 cameraDestPos  {0.0f};
};