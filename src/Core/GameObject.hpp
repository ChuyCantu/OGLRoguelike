#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Components.hpp"
#include "Scene.hpp"

#include <entt/entity/registry.hpp>
#include <string>

//! GameObjects should NOT be stored as pointers, storing their entity and using Scene::FindGameObject is preferred in order to prevent dangling pointers
class GameObject {
public:
    GameObject(Scene* scene, const std::string& name = "GameObject");          
    virtual ~GameObject();

    entt::entity Entity() const { return entity; }

    virtual void Start() {}
    virtual void Update() {}
    virtual void OnEnable() {}
    virtual void OnDisable() {}
    virtual void OnDestroy() {}

    // All ImGui calls should be made here
    virtual void DebugGUI() { }

    void Destroy();

    bool IsActive() const { return isActive; }
    void SetActive(bool value);

    // Messages
    // virtual void OnCollision(const Collider& other)   { }
    virtual void OnCollisionEnter(const Collider& other) { }
    virtual void OnCollisionStay(const Collider& other)  { }
    virtual void OnCollisionExit(const Collider& other)  { }

    template <class Component, class... Args>
    Component& AddCommponent(Args&&... args) {
        Component& c {scene->entityRegistry.emplace<Component>(entity, std::forward<Args>(args)...)};
        c.gameobject = this;
        return c;
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

    GameObject* FindGameObject(std::string& name);
    GameObject* FindGameObject(entt::entity entity);

    bool operator==(const GameObject& other);

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