#include "Engine.hpp"

#include "Log.hpp"
#include "Time.hpp"
#include "Rendering/Renderer.hpp"

#include <glm/ext/vector_int2.hpp>

Engine::Engine(const std::string& title, int width, int height) 
    : state{GameState::Running}, 
      renderer{MakeOwned<Renderer>(this, glm::ivec2{width, height}, title)} {

    LOG_INFO(renderer->GetGraphicsInfo());

    LoadData();
}

Engine::~Engine() {
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
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Shutdown();
                break;
            
            case SDL_KEYDOWN:
                if (!event.key.repeat)
                    HandleKeyPress(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                HandleKeyPress(event.button.button);
                break;
            // TODO: Update renderer viewport on screen size change
        }
    }
}

void Engine::HandleKeyPress(int key) {
    switch (key)
    {
    case SDLK_ESCAPE:
        Shutdown();
        break;
    
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

