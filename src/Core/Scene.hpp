#pragma once

#include "Common.hpp"

#include <entt/entity/registry.hpp>
#include <vector>

class Engine;
class GameObject;

class Scene {
public:
    Scene(Engine* engine);
    virtual ~Scene();

    /**
     * @brief Add an already constructed gameobject to the scene and return a pointer to it
     * 
     * @param gameobject The gameobject to add to the scene
     */
    GameObject* AddGameObject(Owned<GameObject> gameobject); 
    
    // // Object must be a class inheriting from GameObject
    // template<class Object>
    // GameObject* AddGameObject() {
    //     gameobjects.emplace_back(MakeOwned<Object>(this));
    //     return gameobjects[gameobjects.size() - 1].get();
    // }

    /**
     * @brief Creates a gameobject of type Object, adds it to the scene and returns a pointer to it
     * 
     * @tparam Object The type of the object to create (must inherit from GameObject)
     * @param args Extra arguments to construct the gameobject (this excludes the scene for convenience)
     */
    template<class Object, class... Args>
    GameObject* AddGameObject(Args&&... args) {
        return gameobjects.emplace_back(MakeOwned<Object>(this, std::forward<Args>(args)...)).get();
        // return gameobjects[gameobjects.size() - 1].get();
    }

    /**
     * @brief Return a view of the specified Components from the scene entity registry
     * 
     * @tparam Component Type of component used to construct the view.
     * @tparam Other Other types of components used to construct the view.
     * @tparam Exclude Types of components used to filter the view.
     * @return A newly created view.
     */
    template <typename Component, typename... Other, typename... Exclude>
    auto ViewComponents(entt::exclude_t<Exclude...> = entt::exclude<>) {
        return entityRegistry.view<Component, Other...>(entt::exclude<Exclude...>);
    }

    /**
     * @brief Find the first GameObject in the scene with a name equal to the one given (this function may be slow)
     * 
     * @param name 
     * @return If a GameObject was found, a pointer to it, nullptr otherwise
     */
    GameObject* FindGameObject(std::string& name);
    /**
     * @brief Find the GameObject associated to the given entity. This function is fast and should be preferred to retrieve GameObjects instead of storing a pointer to them.
     * 
     * @param entity 
     * @return If the entity is valid, a pointer to the associated GameObject, nullptr otherwise
     */
    GameObject* FindGameObject(entt::entity entity);

    // All ImGui calls should be made here
    virtual void DebugGUI() { }

    Engine* GetEngine() { return engine; }

protected:
    virtual void LastUpdate() {}

private:
    void Update();
    void Render();

    void UpdateGameObjects();
    
protected:
    Engine* engine;

    entt::registry entityRegistry;
    std::vector<Owned<GameObject>> gameobjects;

    bool isAnyGameObjectDead {false};
    bool firstLoop           {true};

    friend class Engine;
    friend class GameObject;
    friend class Renderer;
    friend class Scene;
};