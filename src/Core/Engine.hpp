#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Common.hpp"
#include "Event.hpp"

enum class GameState {
    Running, 
    Paused,
    Quit
};

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

public:
    Event<void(int, int)> OnWindowSizeChanged;

private:
    GameState state;
    Owned<class Renderer> renderer;
};


#endif // __ENGINE_H__