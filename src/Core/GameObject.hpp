#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Components.hpp"
#include "Scene.hpp"

#include <entt/entity/registry.hpp>
#include <string>

class GameObject {
public:
    GameObject(Scene* scene, const std::string& name = "GameObject");          
    virtual ~GameObject();

    virtual void Start() {}
    virtual void Update() {}
    virtual void OnEnable() {}
    virtual void OnDisable() {}
    virtual void OnDestroy() {}

    void Destroy();

    bool IsActive() const { return isActive; }
    void SetActive(bool value);

    // Messages
    virtual void OnCollision(const Collider& other) {}


    template <class Component>
    Component& AddCommponent() {
        Component& c {scene->entityRegistry.emplace<Component>(entity)};
        c.gameobject = this;
        return c;
    }

    template <class Component, class... Args>
    Component& AddCommponent(Args&&... args) {
        return scene->entityRegistry.emplace<Component>(entity, this, std::forward<Args>(args)...);
    }

    template <class Component>
    Component& GetComponent() {
        return scene->entityRegistry.get<Component>(entity);
    }

    template <class Component>
    bool TryGetComponent(Component* component) {
        component = scene->entityRegistry.try_get<Component>(entity);
        if (component)
            return true;
        return false;
    }

    template <class Component>
    void RemoveComponent() {
        if (HasComponents<Component>())
            scene->entityRegistry.remove<Component>(entity);
    }

    template <class Component>
    bool HasComponent() {
        return Scene::GetActiveScene().EntityRegistry.all_of<Component>(entity);
    }

    template <class... Components>
    bool HasComponents(bool matchAll = true) {
        if (matchAll)
            return Scene::GetActiveScene().EntityRegistry.all_of<Components...>(entity);
        else
            return Scene::GetActiveScene().EntityRegistry.any_of<Components...>(entity);
    }

    GameObject* Find(std::string& name);

public:
    std::string name;
    std::string tag;

protected:
    Scene* scene;
    const entt::entity entity;

protected:
    bool hasStarted         {false};
    bool isActive           {true};
    bool isAlive            {true};
    bool DoNotDestroyOnLoad {false};  // Not yet implemented

    friend class Scene;
};

#endif // __GAMEOBJECT_H__