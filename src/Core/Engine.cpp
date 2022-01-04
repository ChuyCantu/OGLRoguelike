#include "Engine.hpp"

#include "AssetsManager.hpp"
#include "Input/Input.hpp"
#include "Log.hpp"
#include "Time.hpp"
#include "Rendering/Renderer.hpp"
#include "Utils/OGLDebug.hpp"

#include <glm/ext/vector_int2.hpp>

Engine::Engine(const std::string& title, int width, int height) 
    : state{GameState::Running}, 
      renderer{MakeOwned<Renderer>(this, glm::ivec2{width, height}, title)} {

    LOG_INFO(renderer->GetGraphicsInfo());

    OGLDebugOutput::Enable(true);

    if (!Input::system->Initialize()) 
        LOG_ERROR("Failed to initialize Input System.");

    LoadData();
}

Engine::~Engine() {
    Input::system->Shutdown();
    UnloadData();
}

void Engine::Run() {
    while (state != GameState::Quit) {
        Time::BeginFrame();

        ProcessInput();
        Update();
        Render();
    }
}

void Engine::Shutdown() {
    state = GameState::Quit;
    LOG_INFO("Shutting down...");
}

void Engine::Pause() {
    state = GameState::Paused;
}

void Engine::ProcessInput() {
    Input::system->PrepareForUpdate();

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Shutdown();
                break;
            case SDL_MOUSEWHEEL:
                Input::system->ProcessEvent(event);
                break;
            case SDL_KEYDOWN:
                if (!event.key.repeat)
                    HandleKeyPress(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                HandleKeyPress(event.button.button);
                break;        
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {                   
                    case SDL_WINDOWEVENT_RESIZED: //* Called after SIZE_CHANGED only on external events (user or window management)
                        // LOG_DEBUG("Window resized w: {}, h: {}.", event.window.data1, event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:  //* Called whenever the window size is changed
                        LOG_DEBUG("Window size changed w: {}, h: {}.", event.window.data1, event.window.data2);
                        OnWindowSizeChanged.Invoke(event.window.data1, event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_MAXIMIZED:
                        LOG_DEBUG("Window maximized");
                        break;
                    case SDL_WINDOWEVENT_MINIMIZED:
                        LOG_DEBUG("Window minimized");
                        break;
                    case SDL_WINDOWEVENT_RESTORED:
                        LOG_DEBUG("Window restored");
                        break;
                }
                break;    
            }
        }
    }

    Input::system->Update();
}

void Engine::HandleKeyPress(int key) {
    switch (key)
    {
    case SDLK_ESCAPE:
        Shutdown();
        break;
    case SDLK_F9: {
        GLint polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
        if (polygonMode == GL_LINE) {
            LOG_INFO("Polygon mode set to: GL_FILL");
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else if (polygonMode == GL_FILL) {
            LOG_INFO("Polygon mode set to: GL_LINE");
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Change to wireframe mode
        }
    }
    case SDLK_F11:
        // const auto& AssetsManager::GetShaders();
        for (auto& [name, shader] : AssetsManager::shaders) 
            shader.HotReload();
    default:
        break;
    }
}

void Engine::Update() {
    // LOG_DEBUG("Time: {}.", Time::time);
    // if (Time::time >= 1.5f) {
    //     Shutdown();
    // }
}

void Engine::Render() {
    renderer->Draw();
}

void Engine::LoadData() {
    
}

void Engine::UnloadData() {
    
}

