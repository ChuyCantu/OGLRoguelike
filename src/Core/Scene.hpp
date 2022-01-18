#ifndef __SCENE_H__
#define __SCENE_H__

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

private:
    void Update();
    void Render();

    void UpdateGameObjects();

    // void CleanFreeQueue();
    
protected:
    Engine* engine;

private:
    entt::registry entityRegistry;
    std::vector<Owned<GameObject>> gameobjects;

    bool isAnyGameObjectDead {false};
    bool firstLoop           {true};

    friend class Engine;
    friend class GameObject;
    friend class Renderer;
};

#endif // __SCENE_H__