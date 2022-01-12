#ifndef __COMPONENTS_H__
#define __COMPONENTS_H__

#include "AssetsManager.hpp"
#include "Common.hpp"
#include "Utils/MathExtras.hpp"

class GameObject;
class Sprite;

struct Component {
    GameObject* gameobject;
};

struct Transform : public Component {
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);

    void UpdateTransform();

    const glm::vec3& GetPosition() const { return position; }
    const glm::quat& GetRotation() const { return rotation; }
    const glm::vec3& GetScale()    const { return scale;    }    
    const glm::mat4& GetModel()    const { return model;    }

private:
    glm::vec3 position {vec3::zero};
    glm::quat rotation {quaternion::identity};
    glm::vec3 scale    {1.0f};
    glm::mat4 model    {1.0f};
    bool isDirty       {true};
};

struct SpriteRenderer : public Component {
    Ref<Sprite> sprite {MakeRef<Sprite>(AssetsManager::GetTexture("default"))};
    glm::vec4 color    {glm::vec4{1.0f}};
    int renderOrder    {0};
};

#endif // __COMPONENTS_H__