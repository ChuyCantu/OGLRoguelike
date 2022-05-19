#include "GameObject.hpp"

#include "Log.hpp"
#include "Scene.hpp"

GameObject::GameObject(Scene* scene, const std::string& name) : name{name}, scene{scene}, entity{scene->entityRegistry.create()} {
    AddComponent<Transform>();
    LOG_DEBUG("GameObject [{}] [entity: {}] created.", name, entt::to_integral(entity));
}

GameObject::~GameObject() {
    Transform& transform {GetComponent<Transform>()};
    if (transform.HasParent()) {
        transform.GetParent()->GetComponent<Transform>().RemoveChild(this);
    }

    transform.RemoveChildren();

    LOG_DEBUG("GameObject [{}] [entity: {}] deleted.", name, entt::to_integral(entity));
    scene->entityRegistry.destroy(entity);
}

void GameObject::SetActive(bool value) {
    if (value && !isActive) {
        isActive = true;
        OnEnable();
    } else if (!value && isActive) {
        isActive = false;
        OnDisable();
    }
}

void GameObject::Destroy() {
    isAlive = false;
    scene->isAnyGameObjectDead = true;

    auto& children {GetComponent<Transform>().GetChildren()};
    if (!children.empty()) {
        for (auto& childEntt : children) {
            GameObject* child {FindGameObject(childEntt)};
            if (child) child->Destroy();
        }
    }

    OnDestroy();
}

GameObject* GameObject::FindGameObject(const std::string& name) {
    return scene->FindGameObject(name);
}

GameObject* GameObject::FindGameObject(entt::entity entity) {
    return scene->FindGameObject(entity);
}

bool GameObject::operator==(const GameObject& other) {
    return entity == other.entity;
}