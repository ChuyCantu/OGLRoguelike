#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Common.hpp"
#include "Event.hpp"

enum class GameState {
    Running, 
    Paused,
    Quit
};

class Scene;
class Renderer;

class Engine {
public:
    Engine(const std::string& title, int width, int height);
    ~Engine();

    void Run();
    void Shutdown();
    void Pause();
    void ProcessInput();
    void HandleKeyPress(int key);
    void Update();
    void Render();
    void LoadData();
    void UnloadData();

    Scene* GetActiveScene() { return activeScene.get(); }
    Renderer* GetRenderer() { return renderer.get(); }

public:
    Event<void(int, int)> OnWindowSizeChanged;

private:
    GameState state;
    Owned<Scene> activeScene;
    Owned<Renderer> renderer;
};


#endif // __ENGINE_H__