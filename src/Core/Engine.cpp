#include "Engine.hpp"

#include "AssetManager.hpp"
#include "Input/Input.hpp"
#include "Log.hpp"
#include "Time.hpp"
#include "Rendering/Batch.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Utils/OGLDebug.hpp"
#include "Scene.hpp"
// #include "Tilemaps/Tile.hpp"

#include "Game/AutotilesLoaders.hpp"
#include "Game/Scenes/Level.hpp"
#include "Game/Tests/TestScene2.hpp"
#include "UI/UI.hpp"
#include "UI/UIStack.hpp"

#ifdef IMGUI
#include <imgui_impl_sdl.h>
#endif // IMGUI
#include <glm/ext/vector_int2.hpp>

Engine* Engine::instance {nullptr};

Engine::Engine(const std::string& title, int width, int height) 
    : state{GameState::Running}, 
      uiStack{},
      renderer{MakeOwned<Renderer>(this, glm::ivec2{width, height}, title)} { 

    instance = this; //  Not the best singleton initialization, but it keep it simple since Engine should be only initialized at main once

    OGLDebugOutput::Enable(true);

    if (!Input::system->Initialize()) 
        LOG_ERROR("Failed to initialize Input System.");

    UI::Init(&uiStack);

    LoadData();

    activeScene = MakeOwned<Level>(this);
}

Engine::~Engine() {
    Input::system->Shutdown();
    UnloadData();
}

Engine& Engine::Instance() { 
    ASSERT(!instance, "No instance of the Engine was created.");
    return *instance; 
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
#ifdef IMGUI
        ImGui_ImplSDL2_ProcessEvent(&event);
#endif // IMGUI
        switch (event.type) {
            case SDL_QUIT:
                Shutdown();
                break;
            case SDL_MOUSEWHEEL:
                Input::system->ProcessEvent(event);
                break;
            case SDL_KEYDOWN:
                if (!event.key.repeat) {
                    HandleKeyPress(event.key.keysym.sym);
                    uiStack.HandleInput(EventHandler{&event, false});
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                HandleKeyPress(event.button.button);
                uiStack.HandleInput(EventHandler{&event, false});
                break;
            case SDL_MOUSEBUTTONUP:
                // HandleKeyPress(event.button.button);
                uiStack.HandleInput(EventHandler{&event, false});
                break;
            case SDL_MOUSEMOTION:
                uiStack.HandleInput(EventHandler{&event, false});
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
    switch (key) {
    case SDLK_ESCAPE:
        Shutdown();
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
    case SDLK_F10:  // Borderless window
        if ((SDL_GetWindowFlags(renderer->GetWindow()) & SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
            SDL_SetWindowFullscreen(renderer->GetWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else {
            SDL_SetWindowFullscreen(renderer->GetWindow(), 0);
        }
        break;
    case SDLK_F11:
        for (auto& [name, shader] : AssetManager::GetShaders()) 
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
    AssetManager::AddTexture("default", MakeRef<Texture>("resources/assets/default_tex.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    AssetManager::AddTexture("missing", MakeRef<Texture>("resources/assets/missing_tex.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);

    //+ Shaders
    AssetManager::AddShader("tilemap", "resources/shaders/tilemap.glsl");
    AssetManager::AddShader("sprite", "resources/shaders/sprite.glsl");
    AssetManager::AddShader("spriteOld", "resources/shaders/spriteOld.glsl");
    AssetManager::AddShader("grid2d", "resources/shaders/grid2d.glsl");
    AssetManager::AddShader("gui", "resources/shaders/gui.glsl");
    AssetManager::AddShader("guiOld", "resources/shaders/guiOld.glsl");
    AssetManager::AddShader("text", "resources/shaders/text.glsl");
    AssetManager::AddShader("textSDF", "resources/shaders/textsdf.glsl");
    AssetManager::AddShader("debug_point", "resources/shaders/debug_point.glsl");

    //+ Assets
    AssetManager::AddTexture("player0_spritesheet", MakeRef<Texture>("resources/assets/Player0.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    AssetManager::AddTexture("player1_spritesheet", MakeRef<Texture>("resources/assets/Player1.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    AssetManager::AddTexture("pit0_spritesheet", MakeRef<Texture>("resources/assets/Pit0.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    AssetManager::AddTexture("pit1_spritesheet", MakeRef<Texture>("resources/assets/Pit1.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    AssetManager::AddTexture("floor_spritesheet", MakeRef<Texture>("resources/assets/Floor.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    AssetManager::AddTexture("wall_spritesheet", MakeRef<Texture>("resources/assets/Wall.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    AssetManager::AddTexture("gui0", MakeRef<Texture>("resources/assets/DawnLike/GUI/GUI0.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    AssetManager::AddTexture("gui1", MakeRef<Texture>("resources/assets/DawnLike/GUI/GUI1.png", true))->SetMinFilter(TextureParameter::Nearest)
        .SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);

    //+ Tile Brushes
    std::unordered_map<int, TileRule> rules;
    Ref<TileBrush> floorTileBrush {MakeRef<TileBrush>()};
    AssetManager::AddTileBrush("floor", floorTileBrush);
    Ref<Texture> floorSpritesheetTex {AssetManager::GetTexture("floor_spritesheet")};
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 0}, 16, rules);
    floorTileBrush->CreateAutoTile("template", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 48}, 16, rules);
    floorTileBrush->CreateAutoTile("01", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{112, 48}, 16, rules);
    floorTileBrush->CreateAutoTile("02", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{224, 48}, 16, rules);
    floorTileBrush->CreateAutoTile("03", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 96}, 16, rules);
    floorTileBrush->CreateAutoTile("04", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{112, 96}, 16, rules);
    floorTileBrush->CreateAutoTile("05", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{224, 96}, 16, rules);
    floorTileBrush->CreateAutoTile("06", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 144}, 16, rules);
    floorTileBrush->CreateAutoTile("07", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{112, 144}, 16, rules);
    floorTileBrush->CreateAutoTile("08", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{224, 144}, 16, rules);
    floorTileBrush->CreateAutoTile("09", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 192}, 16, rules);
    floorTileBrush->CreateAutoTile("10", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{112, 192}, 16, rules);
    floorTileBrush->CreateAutoTile("11", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{224, 192}, 16, rules);
    floorTileBrush->CreateAutoTile("12", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 240}, 16, rules);
    floorTileBrush->CreateAutoTile("13", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{112, 240}, 16, rules);
    floorTileBrush->CreateAutoTile("14", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{224, 240}, 16, rules);
    floorTileBrush->CreateAutoTile("15", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 288}, 16, rules);
    floorTileBrush->CreateAutoTile("16", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{112, 288}, 16, rules);
    floorTileBrush->CreateAutoTile("17", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{224, 288}, 16, rules);
    floorTileBrush->CreateAutoTile("18", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 336}, 16, rules);
    floorTileBrush->CreateAutoTile("19", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{112, 336}, 16, rules);
    floorTileBrush->CreateAutoTile("20", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{224, 336}, 16, rules);
    floorTileBrush->CreateAutoTile("21", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 384}, 16, rules);
    floorTileBrush->CreateAutoTile("22", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{112, 384}, 16, rules);
    floorTileBrush->CreateAutoTile("23", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{224, 384}, 16, rules);
    floorTileBrush->CreateAutoTile("24", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 432}, 16, rules);
    floorTileBrush->CreateAutoTile("25", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 480}, 16, rules);
    floorTileBrush->CreateAutoTile("26", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 528}, 16, rules);
    floorTileBrush->CreateAutoTile("27", rules[15].sprites[0], rules);
    CreateAutotileRules4Dir(floorSpritesheetTex, glm::ivec2{0, 576}, 16, rules);
    floorTileBrush->CreateAutoTile("28", rules[15].sprites[0], rules);

    //+ Init Batch Renderers
    // int textureUnits;
    // glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
    // SpriteBatch::Init(textureUnits);
    // TextBatch::Init();
    InitBatchRenderers();
}

void Engine::UnloadData() {
    
}

