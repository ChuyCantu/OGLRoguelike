#include "Components.hpp"

#include "Log.hpp"
#include "Rendering/Texture.hpp"
#include "GameObject.hpp"
#include "Time.hpp"
#include "Rendering/Batch.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Shader.hpp"
#include "Input/Input.hpp"
#include "Utils/Random.hpp"

#include <glm/gtc/matrix_transform.hpp>

//+ Transform =================================================================

void Transform::SetPosition(const glm::vec3& position) {
    this->position = position;
    isDirty = true;

    if (HasParent()) {
        Transform& parentTransform {gameobject->FindGameObject(parent)->GetComponent<Transform>()};
        absolutePosition = parentTransform.absolutePosition + this->position;
    }
    else {
        absolutePosition = this->position;
    }

    if (!children.empty())
        UpdateChildrenPosition();
}

void Transform::SetPosition(const glm::vec2& position) {
    this->position.x = position.x;
    this->position.y = position.y;
    this->position.z = 0.0f;
    isDirty = true;

    if (HasParent()) {
        Transform& parentTransform {gameobject->FindGameObject(parent)->GetComponent<Transform>()};
        absolutePosition = parentTransform.absolutePosition + this->position;
    } 
    else {
        absolutePosition = this->position;
    }

    if (!children.empty())
        UpdateChildrenPosition();
}

void Transform::SetAbsolutePosition(const glm::vec3& position) {
    this->position = position;
    isDirty = true;

    absolutePosition = this->position;

    if (!children.empty())
        UpdateChildrenPosition();
}

void Transform::SetAbsolutePosition(const glm::vec2& position) {
    this->position.x = position.x;
    this->position.y = position.y;
    this->position.z = 0.0f;
    isDirty = true;

    absolutePosition = this->position;

    if (!children.empty())
        UpdateChildrenPosition();
}

void Transform::SetRotation(const glm::quat& rotation) {
    this->rotation = rotation;
    isDirty = true;
}

void Transform::SetScale(const glm::vec3& scale) {
    this->scale = scale;
    isDirty = true;
}

void Transform::SetScale(const glm::vec2& scale) {
    this->scale.x = scale.x;
    this->scale.y = scale.y;
    this->scale.z = 0.0f;
    isDirty = true;
}

void Transform::AddChild(GameObject* gameobject) {
    children.emplace_back(gameobject->Entity());
    Transform& childTransform {gameobject->GetComponent<Transform>()};
    childTransform.parent = this->gameobject->Entity();
    childTransform.position = childTransform.absolutePosition - absolutePosition;
}

void Transform::RemoveChild(GameObject* gameobject) {
    auto iter {std::find(children.begin(), children.end(), gameobject->Entity())};
    if (iter != children.end()) {
        children.erase(iter);
        Transform& childTransform {gameobject->GetComponent<Transform>()};
        childTransform.parent = entt::null;
        childTransform.position = childTransform.absolutePosition;
    }
}

void Transform::RemoveChildren() {
    for (auto& childEntity : children) {
        GameObject* childGo {gameobject->FindGameObject(childEntity)};
        if (childGo) {
            Transform& childTransform {childGo->GetComponent<Transform>()};
            childTransform.parent = entt::null;
            childTransform.position = childTransform.absolutePosition;
        }
    }

    children.clear();
}

void Transform::UpdateRelativePosition() {
    SetPosition(position);
}

void Transform::UpdateChildrenPosition() {
    GameObject* child {nullptr};
    for (auto& childEntity : children) {
        child = gameobject->FindGameObject(childEntity);
        if (child)
            child->GetComponent<Transform>().UpdateRelativePosition();
    }
}

void Transform::UpdateTransform() {
    if (isDirty) {
        glm::mat4 translation {glm::translate(glm::mat4{1.0}, absolutePosition)};
        glm::mat4 rotation    {glm::mat4_cast(this->rotation)};
        glm::mat4 scale       {glm::scale(glm::mat4{1.0f}, this->scale)};
        model = translation * rotation * scale;
    }
}

GameObject* const Transform::GetParent() const { 
    return gameobject->FindGameObject(parent); 
}

//+ SpriteRenderer =================================================================
SpriteRenderer::SpriteRenderer() : sprite{MakeRef<Sprite>(AssetManager::GetTexture("missing"))} { }
SpriteRenderer::SpriteRenderer(Ref<Sprite> sprite, Color color, int renderOrder, const glm::vec2& pivot)
    : sprite{sprite}, color{color}, renderOrder{renderOrder}, pivot{pivot} { }

//+ ColliderComponent =================================================================
Collider::Collider(bool isSolid, bool ignoreSolid) : isSolid{isSolid}, ignoreSolid{ignoreSolid} { }

//+ TilemapCollider =================================================================
TilemapCollider::TilemapCollider(bool isSolid) : isSolid{isSolid} { }

//+ MoveComponent =================================================================

// TODO: Give constructor to every Component so gameobject only have one AddComponent method
void MoveComponent::Move(glm::vec3 destination, float duration) {
    srcPosition = gameobject->GetComponent<Transform>().GetAbsolutePosition();
    destPosition = destination;
    timer = 0.f;
    time = duration;
    startedMove = true;

    // ------
    if (std::abs(destination.x - srcPosition.x) <= 0.001f)
        shouldJump = false;
    else
        shouldJump = true;

    // destPosition1 = Lerp(srcPosition, destPosition, 0.5f) + glm::vec3{0.0f, 6.0f, 0.0f};
    // destPosition2 = destination;
    // timeH = duration / 2;

    if (shouldJump) {
        destPosition1 = Lerp(srcPosition, destPosition, 0.5f) + glm::vec3{0.0f, 6.0f, 0.0f};
        destPosition2 = destination;
        timeH = duration / 2;
    }
    else {
        // destPosition1 = Lerp(srcPosition, destPosition, 0.5f) + glm::vec3{0.0f, 6.0f, 0.0f};
        if (destination.y - srcPosition.y > 0)
            destPosition1 = destination + glm::vec3{0.0f, 2.0f, 0.0f};
        else
            destPosition1 = srcPosition + glm::vec3{0.0f, 2.0f, 0.0f};

        destPosition2 = destination;
        timeH = duration / 2;
    }
    
}

void MoveComponent::Teleport(glm::vec3 destination) {
    srcPosition = destination;
    destPosition = destination;
    gameobject->GetComponent<Transform>().SetPosition(destination);
}

void MoveComponent::Update() {
    // if (!shouldJump) {
    //     if (timer <= time) {
    //         if (startedMove) startedMove = false;
    //         auto& transform {gameobject->GetComponent<Transform>()};
    //         transform.SetPosition(Lerp(srcPosition, destPosition, timer / time));
    //         timer += Time::deltaTime;
    //     }
    //     else {
    //         auto& transform{gameobject->GetComponent<Transform>()};
    //         transform.SetPosition(destPosition);
    //         srcPosition = destPosition;
    //         onDestinationReached.Invoke();
    //     }
    // }
    // else {
        if (!reachedDest1) {
            if (timer <= timeH) {
                if (startedMove) startedMove = false;
                auto& transform{gameobject->GetComponent<Transform>()};
                transform.SetPosition(Lerp(srcPosition, destPosition1, timer / timeH));
                timer += Time::deltaTime;
            } 
            else {
                auto& transform{gameobject->GetComponent<Transform>()};
                transform.SetPosition(destPosition1);
                srcPosition = destPosition1;
                timer = 0;
                reachedDest1 = true;
            }
        }
        else {
            if (timer <= timeH) {
                if (startedMove) startedMove = false;
                auto& transform{gameobject->GetComponent<Transform>()};
                transform.SetPosition(Lerp(destPosition1, destPosition2, timer / timeH));
                timer += Time::deltaTime;
            } 
            else {
                auto& transform{gameobject->GetComponent<Transform>()};
                transform.SetPosition(destPosition2);
                srcPosition = destPosition2;
                onDestinationReached.Invoke();
                reachedDest1 = false;
            }
        }
    // }
}

void MoveComponent::Cancel() {
    destPosition = srcPosition;
    gameobject->GetComponent<Transform>().SetPosition(srcPosition);
    time = 0.f;
    onCancelation.Invoke();
}