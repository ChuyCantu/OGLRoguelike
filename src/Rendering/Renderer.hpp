#ifndef __RENDERER_H__
#define __RENDERER_H__

#define IMGUI

#include "Buffer.hpp"
#include "Common.hpp"
#include "TilemapRenderer.hpp"
#include "UniformBuffer.hpp"

#include <glm/ext/vector_int2.hpp>
#include <SDL.h>

class Renderer {
public:
    Renderer(class Engine* engine, glm::ivec2 screenSize, const std::string& windowTitle);
    ~Renderer();

    void LoadData();
    void Draw();
    void SetViewport(int x, int y, int width, int height);
    void SetScreenSize(int width, int height);
    void SetVirtualScreenSize(int width, int height);

    // Utils
    std::string GetGraphicsInfo();

    // For events
    // void OnWindowResized(glm::ivec2 size);
    void OnWindowSizeChanged(int width, int height);

public: 
    const glm::ivec2& screenSize {_screenSize};
    const glm::ivec2& virtualScreenSize {_virtualScreenSize};

private:
    glm::ivec2 _screenSize;
    glm::ivec2 _virtualScreenSize;

    SDL_Window* window;
    SDL_GLContext context;

    class Engine* engine;

#ifdef IMGUI
    struct ImGuiIO* io;
#endif  // IMGUI

    //! Debug
    Owned<TilemapRenderer> tilemapRenderer;
    // UniformBuffer uniformBufferExample;
    UniformBuffer globalsUBO;
};

#endif // __RENDERER_H__

//+ globalsBuffer binding = 0:
//* vec2 screenSize  - 8  (2N)
//* vec2 viewport    - 8  (2N)
//* mat4 projection  - 64 (16N)
//*                  - 80