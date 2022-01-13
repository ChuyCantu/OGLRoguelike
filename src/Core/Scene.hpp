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

    GameObject* AddGameObject(Owned<GameObject> gameobject); 
    
    // Object must be a class inheriting from GameObject
    template<class Object>
    GameObject* AddGameObject() {
        gameobjects.emplace_back(MakeOwned<Object>(this));
        return gameobjects[gameobjects.size() - 1].get();
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

    bool haveObjectsStarted  {false};
    bool isAnyGameObjectDead {false};

    friend class Engine;
    friend class GameObject;
    friend class Renderer;
};

#endif // __SCENE_H__