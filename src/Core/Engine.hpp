#pragma once

#include "Common.hpp"
#include "Event.hpp"
#include "UI/UIStack.hpp"

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
    UIStack* GetUIStack()   { return &uiStack; }
    static Engine& Instance();

public:
    Event<void(int, int)> OnWindowSizeChanged;

private:
    GameState state;
    UIStack uiStack;
    Owned<Renderer> renderer;
    Owned<Scene> activeScene;
    

    static Engine* instance;
};