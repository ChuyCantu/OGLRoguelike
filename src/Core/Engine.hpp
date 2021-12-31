#ifndef __ENGINE_H__
#define __ENGINE_H__

#define IMGUI

#include "Common.hpp"

#include <SDL.h>

enum class GameState {
    Running, 
    Paused,
    Quit
};

class Engine {
private:
    GameState state;

    // TODO: Move to renderer
    int screenWidth; 
    int screenHeight;
    SDL_Window* window;
    SDL_GLContext context;

#ifdef IMGUI
    struct ImGuiIO* io;
#endif // IMGUI

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

    // TODO: Move to renderer
    float GetScreenWidth() const { return screenWidth; }
    float GetScreenHeight() const { return screenHeight; }
};


#endif // __ENGINE_H__