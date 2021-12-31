#include "Renderer.hpp"

#include "Core/Engine.hpp"
#include "Core/Log.hpp"
#include "Core/Time.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"

#include <fmt/core.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#ifdef IMGUI
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#endif  // IMGUI

Renderer::Renderer(Engine* engine, glm::ivec2 screenSize, const std::string& windowTitle) 
    : _screenSize{screenSize}, engine{engine} {
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG_CRITICAL("Could not initialize SDL: {}.", SDL_GetError());
        engine->Shutdown();
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    // Request color buffer with 8-bit per channel
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // Enable double-buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // Use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    //! Not necessary for this project:
    // // Depth buffer of 24-bits
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // // Stencil buffer of 8 bits
    // SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    // // Multi-sample anti-aliasing
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow(windowTitle.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              _screenSize.x, _screenSize.y,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (!window) {
        LOG_CRITICAL("Failed to create Window: {}.", SDL_GetError());
        engine->Shutdown();
        return;
    }

    // // Set window icon (needs SLD_image)
    // SDL_Surface* icon{IMG_Load("assets/Logo-64.png")};
    // if (!icon)
    //     SDL_Log("Could not create window icon.");
    // else {
    //     SDL_SetWindowIcon(window, icon);
    //     SDL_FreeSurface(icon);
    // }

    context = SDL_GL_CreateContext(window);
    if (!context) {
        LOG_CRITICAL("Failed to create OpenGL Context: {}.", SDL_GetError());
        engine->Shutdown();
        return;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        LOG_CRITICAL("Failed to initialize GLAD.");
        engine->Shutdown();
        return;
    }

    glViewport(0, 0, _screenSize.x, _screenSize.y);

#ifdef IMGUI
    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // io->IniFilename = nullptr; // Disables ini saving file (layout file)

    ImGui::StyleColorsDark();

    // Platform bindings
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 130");
#endif  // IMGUI

    LoadData();
}

Renderer::~Renderer() {
    shader.Unload();

#ifdef IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif  // IMGUI

    // SDL
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::LoadData() {
    float vertices[]{
        // pos      //color             // // tex
        -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, // 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // 0.0f, 0.0f,

        -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, // 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f  // 1.0f, 0.0f
    };

    // float vertices[]{
    //         // pos      //color            // // tex
    //         0.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // 0.0f, 1.0f,
    //         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // 1.0f, 0.0f,
    //         -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // 0.0f, 0.0f,
    // };

    // float vertices[]{
    //     // pos      //color            // // tex
    //     0.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // 0.0f, 1.0f,
    //     1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // 1.0f, 0.0f,
    //     0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // 0.0f, 0.0f,

    //     0.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // 0.0f, 1.0f,
    //     1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  // 1.0f, 1.0f,
    //     1.0f, 0.0f, 0.0f, 1.0f, 0.0f   // 1.0f, 0.0f
    // };

    vao = MakeOwned<VertexArray>(vertices, 6, nullptr, 0, VertexArray::Layout::UV | VertexArray::Layout::Normal); // Layout names are wrong, this will be changed later
    shader.Load("resources/shaders/sample.glsl");
}

void Renderer::Draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

#ifdef IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
#endif  // IMGUI

    //* Render anything in here
    vao->Use();
    shader.Use();
    glDrawArrays(GL_TRIANGLES, 0, vao->GetVerticesCount());

#ifdef IMGUI
    // ImGui::ShowDemoWindow();

// FPS Counter: ================================
    static float fpsCounter{0.5f};
    static float fps{0};
    fpsCounter += Time::deltaTime;
    if (fpsCounter >= 0.5f) {
        fpsCounter -= 0.5f;
        fps = 1.f / Time::deltaTime;
    }
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::SetNextWindowPos(ImVec2{_screenSize.x - 2.f, 2.f}, 0, ImVec2{1.f, 0.f});
    ImGui::SetNextWindowSize(ImVec2{55, 15});
    ImGui::Begin("FPS", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    ImGui::Text("%s fps", std::to_string((int)fps).c_str());
    ImGui::End();
// ============================================

    ImGui::Render();
    // glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif  // IMGUI

    SDL_GL_SwapWindow(window);
}

void Renderer::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

std::string Renderer::GetGraphicsInfo() {
    return fmt::format("\n\tGraphics Info:\n"
                       "\tVendor:          {}\n"
                       "\tGPU:             {}\n"
                       "\tDrivers Version: {}\n",
                       glGetString(GL_VENDOR), 
                       glGetString(GL_RENDERER), 
                       glGetString(GL_VERSION));
}

void Renderer::OnWindowResized(glm::ivec2 size) {
    _screenSize = size;
    glViewport(0, 0, _screenSize.x, _screenSize.y);
}