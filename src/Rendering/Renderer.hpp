#ifndef __RENDERER_H__
#define __RENDERER_H__

#define IMGUI

#include <SDL.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Common.hpp"
#include "FrameBuffer.hpp"

class Renderer {
public:
    Renderer(class Engine* engine, glm::ivec2 screenSize, const std::string& windowTitle, bool fullscreen = false);
    ~Renderer();

    void LoadData();
    void Draw();
    void SetViewport(int x, int y, int width, int height);
    void SetScreenSize(int width, int height);
    // void SetVirtualScreenSize(int width, int height);

    // Utils
    std::string GetGraphicsInfo();

    //! Temp
    SDL_Window* GetWindow() const { return window; }

    // For events
    // void OnWindowResized(glm::ivec2 size);
    void OnWindowSizeChanged(int width, int height);

    //! Debug
    void DrawLine2D(const glm::vec2& p1, const glm::vec2& p2, const glm::vec3& color);

public: 
    const glm::ivec2& screenSize {_screenSize};
    // const glm::ivec2& virtualScreenSize {_virtualScreenSize};

    //! Temp
    bool fullscreen;

private:
    glm::ivec2 _screenSize;
    // glm::ivec2 _virtualScreenSize;

    SDL_Window* window;
    SDL_GLContext context;

    class Engine* engine;

#ifdef IMGUI
    struct ImGuiIO* io;
#endif  // IMGUI

    //! Debug
    Framebuffer defaultFBO;

    //! Only meant for debugging:
    struct LinePoint2D {
        glm::vec2 position;
        glm::vec3 color;
    };

    std::vector<LinePoint2D> points;
    Owned<class VertexArray> pointsData;
};

#endif // __RENDERER_H__

//+ Globals UniformBuffer binding = 0:
//*                           size   |  offset 
//* ivec2 screenSize        - 8  (2N)      0
//* ivec2 virtualScreenSize - 8  (2N)      8
//* mat4 projection         - 64 (16N)     16
//* mat4 view               - 64 (16N)     80
//* mat4 projView           - 64 (16N)     144
//*                         - 208          208