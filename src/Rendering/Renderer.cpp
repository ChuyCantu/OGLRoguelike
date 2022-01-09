#include "Renderer.hpp"

#include "Camera.hpp"
#include "Core/AssetsManager.hpp"
#include "Core/Engine.hpp"
#include "Core/Log.hpp"
#include "Core/Time.hpp"

#include <fmt/core.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef IMGUI
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#endif  // IMGUI

Renderer::Renderer(Engine* engine, glm::ivec2 screenSize, const std::string& windowTitle, bool fullscreen) 
    : _screenSize{screenSize}, _virtualScreenSize{640, 320}, fullscreen{fullscreen}, engine{engine} {
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG_CRITICAL("Could not initialize SDL: {}.", SDL_GetError());
        engine->Shutdown();
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
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

    //+ Events
    engine->OnWindowSizeChanged.Subscribe("WindowSizeChanged", &Renderer::OnWindowSizeChanged, this);
}

Renderer::~Renderer() {
    //+ Events
    engine->OnWindowSizeChanged.Unsubscribe("WindowSizeChanged", this);

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

#include "VertexArray.hpp"
#include "Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
void Renderer::LoadData() {
    auto globalsUBO {AssetsManager::AddBuffer("Globals", MakeRef<UniformBuffer>(208, 0))};
    globalsUBO->SetData(0, 8, glm::value_ptr(_screenSize));

    auto mainCamera {MakeRef<Camera>(glm::ivec2{640, 360}, this)};
    Camera::SetMainCamera(mainCamera);

    // globalsUBO.Create(80, 0);
    // globalsUBO.SetData(0, 8, glm::value_ptr(_screenSize));
    // globalsUBO.SetData(8, 8, glm::value_ptr(_virtualScreenSize));
    // // glm::mat4 projection{glm::ortho(0.f, (float)_virtualScreenSize.x, 0.f, (float)_virtualScreenSize.y)};
    // glm::mat4 projection{glm::ortho(-(float)_virtualScreenSize.x / 2.0f, (float)_virtualScreenSize.x / 2.0f, -(float)_virtualScreenSize.y / 2.0f, (float)_virtualScreenSize.y / 2.0f)};
    // globalsUBO.SetData(16, sizeof(glm::mat4), glm::value_ptr(projection));
    
    tilemapRenderer = MakeOwned<TilemapRenderer>(32, 60, 16);

    auto playerTex {AssetsManager::AddTexture("player0_spritesheet", MakeRef<Texture>("resources/assets/Player0.png", true))};
    playerTex->SetMinFilter(TextureParameter::Nearest).SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
    spriteTest = MakeRef<Sprite>(AssetsManager::GetTexture("player0_spritesheet"), glm::ivec2{0, 112}, glm::ivec2{16, 16});
    AssetsManager::AddShader("sprite", "resources/shaders/sprite.glsl");

    std::vector<float> spriteVert { 
        0.f, 0.f, // down-left
        0.f, 1.f, // top-left
        1.f, 0.f, // down-right
        1.f, 1.f  // rop-right
    };
    VertexLayout spriteLayout {
        VertexElement {2, DataType::Float}
    };
    spriteVAO = MakeRef<VertexArray>(spriteVert.data(), 4, spriteLayout);
    spriteVAO->SetDrawMode(DrawMode::TriangleStrip);
}

void Renderer::Draw() {
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);
    float clearColor[] { 0.0f, 0.0f, 0.0f, 1.0f };
    defaultFBO.ClearColor(clearColor);

#ifdef IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
#endif  // IMGUI

    //+ Render anything in here ===============================================

    tilemapRenderer->Draw();

    //! Sprite rendering example
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    auto spriteShader {AssetsManager::GetShader("sprite")};
    spriteShader->Use();
    spriteVAO->Use();
    AssetsManager::GetTexture("player0_spritesheet")->Use();
    glm::mat4 model { glm::translate(glm::mat4{1.f}, glm::vec3{0.f})};// glm::vec3{-310.f, -150.f, 0.f})};
    // model = glm::scale(model, glm::vec3(4.f, 4.f, 1.f));
    spriteShader->SetMatrix4("model", model);
    spriteShader->SetVec2("spriteMinUV", spriteTest->GetMinUV());
    spriteShader->SetVec2("spriteMaxUV", spriteTest->GetMaxUV());
    spriteShader->SetIVec2("spriteSize", spriteTest->GetSize());
    spriteShader->SetVec4("color", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    spriteVAO->Draw();
    glDisable(GL_BLEND);

    //+ =======================================================================

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
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar 
                 | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
    ImGui::Text("%s fps", std::to_string((int)fps).c_str());
    ImGui::PopStyleColor();
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

void Renderer::SetScreenSize(int width, int height) {
    _screenSize = glm::ivec2(width, height);
    SDL_SetWindowSize(window, width, height);
}

void Renderer::SetVirtualScreenSize(int width, int height) {
    _virtualScreenSize = glm::ivec2(width, height);
    // globalsUBO.SetData(8, 8, glm::value_ptr(_virtualScreenSize));
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

void Renderer::OnWindowSizeChanged(int width, int height) {
    _screenSize = glm::ivec2{width, height};
    glViewport(0, 0, width, height);
    // globalsUBO.SetData(0, 8, glm::value_ptr(_screenSize));
}