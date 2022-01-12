#include "Engine.hpp"

#include "AssetsManager.hpp"
#include "Input/Input.hpp"
#include "Log.hpp"
#include "Time.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering//Shader.hpp"
#include "Utils/OGLDebug.hpp"
#include "Scene.hpp"

#include "Game/TestScene.hpp"

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
    case SDLK_F7: // Borderless window
        if ((SDL_GetWindowFlags(renderer->GetWindow()) & SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
            SDL_SetWindowFullscreen(renderer->GetWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else {
            SDL_SetWindowFullscreen(renderer->GetWindow(), 0);
        }
        break;
    case SDLK_F8: // Fullscreen
        // if (renderer->fullscreen) {
        //     renderer->fullscreen = false;
        //     SDL_SetWindowFullscreen(renderer->GetWindow(), 0);
        //     SDL_SetWindowSize(renderer->GetWindow(), 960, 540);
        // }
        // else {
        //     renderer->fullscreen = true;
        //     SDL_SetWindowFullscreen(renderer->GetWindow(), SDL_WINDOW_FULLSCREEN);
        //     SDL_SetWindowSize(renderer->GetWindow(), 1920, 1080);
        // }
        break;
    case SDLK_F9: { // Polygon mode
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
        for (auto& [name, shader] : AssetsManager::GetShaders()) 
            shader->HotReload();
    default:
        break;
    }
}

void Engine::Update() {
    activeScene->Update();
}

void Engine::Render() {
    renderer->Draw();
}

void Engine::LoadData() {
    //+ Defaults
    AssetsManager::AddTexture("default", MakeRef<Texture>("resources/assets/default_texture.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);

    activeScene = MakeOwned<TestScene>(this);
}

void Engine::UnloadData() {
    
}

