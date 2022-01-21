#include "GameObject.hpp"

#include "Log.hpp"
#include "Scene.hpp"

GameObject::GameObject(Scene* scene, const std::string& name) : name{name}, scene{scene}, entity{scene->entityRegistry.create()} {
    AddCommponent<Transform>();
    LOG_DEBUG("GameObject [{}] [entity: {}] created.", name, entt::to_integral(entity));
}

GameObject::~GameObject() {
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
    OnDestroy();
}

GameObject* GameObject::Find(std::string& name) {
    for (auto& gameobject : scene->gameobjects) {
        if (gameobject->name == name)
            return gameobject.get();
    }
    return nullptr;
}

bool GameObject::operator==(const GameObject& other) {
    return entity == other.entity;
}