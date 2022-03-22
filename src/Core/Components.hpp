#pragma once

#include "AssetManager.hpp"
#include "Common.hpp"
#include "Event.hpp"
#include "Log.hpp"
#include "Utils/MathExtras.hpp"
#include "Utils/Color.hpp"

#include <entt/entity/registry.hpp>

class GameObject;
class Sprite;
class Texture;

struct Component {
    GameObject* gameobject {nullptr};
    bool enabled {true};
};

struct Transform : public Component {
    Transform() { }

    void SetPosition(const glm::vec3& position);
    void SetPosition(const glm::vec2& position);
    void SetAbsolutePosition(const glm::vec3& position);
    void SetAbsolutePosition(const glm::vec2& position);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);
    void SetScale(const glm::vec2& scale);

    void AddChild(GameObject* gameobject);
    void RemoveChild(GameObject* gameobject);
    void RemoveChildren();

    void UpdateChildrenPosition();
    void UpdateRelativePosition();
    void UpdateTransform();

    const glm::vec3& GetPosition()         const { return position; }
    const glm::vec3& GetAbsolutePosition() const { return absolutePosition; }
    const glm::quat& GetRotation()         const { return rotation; }
    const glm::vec3& GetScale()            const { return scale;    }    
    const glm::mat4& GetModel()            const { return model;    }
    GameObject* const GetParent()          const;
    entt::entity GetParentEntity()         const { return parent; }
    bool HasParent()                       const { return parent != entt::null; }
    const std::vector<entt::entity>& GetChildren() const { return children; }

private:
    glm::vec3 position         {vec3::zero};
    glm::vec3 absolutePosition {vec3::zero};
    glm::quat rotation         {quaternion::identity};
    glm::vec3 scale            {1.0f}; // For objects whose pivot is not in the center (like Sprite), negative values will change the position relative to that pivot, consider flip instead
    glm::mat4 model            {1.0f};
    bool isDirty               {true};

    entt::entity parent        {entt::null};
    std::vector<entt::entity> children;

    using Component::enabled;
};

struct SpriteRenderer : public Component {
    SpriteRenderer();
    SpriteRenderer(Ref<Sprite> sprite, Color color = ColorNames::white, int renderOrder = 0, const glm::vec2& pivot = glm::vec2{0.0f, 0.0f});

    Ref<Sprite> sprite; //{MakeRef<Sprite>(AssetManager::GetTexture("missing"))};
    Color color        {ColorNames::white};
    int renderOrder    {0};
    // (0, 0) is bottom-left corner
    glm::vec2 pivot    {0.0f, 0.0f};
};

struct Animator : public Component { // For this game, something this simple will do the job
    struct Frame
    {
        Ref<Texture> texture;
        float duration;
    };

    Animator() { }

    float timer           {0.0f};
    uint32_t currentFrame {0};
    std::vector<Frame> frames;
};

// Collider vs Collider will check if the location to move the object is occupied, if it is, there is a collision and may or not occupy the same space
// Collider vs TilemapCollider will check if the tile is different to 0 at the destination position, if it is, then there is a collision
struct Collider : public Component {
    Collider(bool isSolid = true, bool ignoreSolid = false);

    bool isSolid       {true};
    bool ignoreSolid   {false};
};

struct TilemapCollider : public Component {
    TilemapCollider(bool isSolid = true);

    bool isSolid {true};
};

//+ Move component for Turn Based system

struct MoveComponent : public Component {
    MoveComponent() { }

    void Move(glm::vec3 destination, float duration);
    void Update();
    void Cancel();
    void Teleport(glm::vec3 destination);

    const glm::vec3& GetSrcPosition() const { return srcPosition; }
    const glm::vec3& GetDestPosition() const { return destPosition; };
    const bool IsMoving() const { return srcPosition != destPosition; }

    Event<void()> onDestinationReached;
    Event<void()> onCancelation;

private:
    glm::vec3 srcPosition  {vec3::zero};
    glm::vec3 destPosition {vec3::zero};
    float timer            {0.0f};
    float time             {0.0f};
    
    bool startedMove       {false};

    friend class Scene;
};