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

    void LoadData();
    void Draw();
    void SetViewport(int x, int y, int width, int height);

    // Utils
    std::string GetGraphicsInfo();

    // For events
    void OnWindowResized(glm::ivec2 size);

public: 
    const glm::ivec2& screenSize {_screenSize};

private:
    glm::ivec2 _screenSize;

    SDL_Window* window;
    SDL_GLContext context;

    class Engine* engine;

#ifdef IMGUI
    struct ImGuiIO* io;
#endif  // IMGUI

    //! Debug
    Owned<class VertexArray> vao;
    Owned<class Shader> shader;
};

#endif // __RENDERER_H__