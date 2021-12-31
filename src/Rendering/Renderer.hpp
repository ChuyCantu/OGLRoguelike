#ifndef __RENDERER_H__
#define __RENDERER_H__

#define IMGUI

#include "Common.hpp"

#include <glm/ext/vector_int2.hpp>
#include <SDL.h>

class Renderer {
public:
    Renderer(class Engine* engine, glm::ivec2 screenSize, const std::string& windowTitle);
    ~Renderer();

    void Draw();
    void SetViewport(int x, int y, int width, int height);

    // Utils
    std::string GetGraphicsInfo();

public: 
    glm::ivec2& screenSize {_screenSize};

private:
    glm::ivec2 _screenSize;

    SDL_Window* window;
    SDL_GLContext context;

    class Engine* engine;

#ifdef IMGUI
    struct ImGuiIO* io;
#endif  // IMGUI


};

#endif // __RENDERER_H__