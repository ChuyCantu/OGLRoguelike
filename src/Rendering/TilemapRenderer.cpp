#include "TilemapRenderer.hpp"

#include "Camera.hpp"
#include "Core/AssetsManager.hpp"
#include "Core/Time.hpp"
#include "Core/Log.hpp"
#include "Input/Input.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Utils/Random.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <entt/entity/registry.hpp>

TilemapRenderer::TilemapRenderer(int w, int h, int tileSize) 
    : width{w}, height{h}, tileSize{tileSize}, tiles(w * h) {
    
    // shader.Load("resources/shaders/tilemap.glsl");
    AssetsManager::AddShader("tilemap", "resources/shaders/tilemap.glsl");
    AssetsManager::AddShader("tilemapv3", "resources/shaders/tilemapv3.glsl");

    for (uint16_t i {0}; i < tiles.size(); ++i) {
        tiles[i] = i; //Random::Range(0, 1919);
    }

    VertexLayout layout {
        VertexElement {1, DataType::UShort, true}
    };
    vao = VertexArray{tiles.data(), static_cast<uint32_t>(tiles.size()), layout, BufferUsage::Dynamic};
    vao.SetDrawMode(DrawMode::Points);

    // texture.Load("resources/assets/TileSetSample.png", true);
    // texture.Load("resources/assets/Font.png", true);
    // texture.Load("resources/assets/Custom2_16x16.png", true);
    // texture.SetMinFilter(TextureParameter::Nearest).SetMagFilter(TextureParameter::Nearest)
    //        .SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);

    auto texture{AssetsManager::AddTexture("tilemap", MakeRef<Texture>("resources/assets/ignore/ASCII Roguelike/16x16.bmp", true))};
    texture->SetMinFilter(TextureParameter::Nearest).SetMagFilter(TextureParameter::Nearest).SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
}

glm::vec3 tilemapPos{-310.f, -120.f, 0.f};
void TilemapRenderer::Draw() 
{
    //! This is only for debuging and it's temporary
    auto& camera{Camera::GetMainCamera()};
    glm::vec3 cameraPos{camera.GetPosition()};

    auto& input = Input::system->GetState().Keyboard;
    // Movement is reversed for the camera
    if (input.GetKeyValue(SDL_SCANCODE_D)) {
        cameraPos.x += 1.f;
    }
    if (input.GetKeyValue(SDL_SCANCODE_A)) {
        cameraPos.x -= 1.f;
    // if (input.GetKeyState(SDL_SCANCODE_A) == ButtonState::Pressed) {
    //    cameraPos.x -= 16.f;
    }
    if (input.GetKeyValue(SDL_SCANCODE_W)) {
        cameraPos.y += 1.f;
    }
    if (input.GetKeyValue(SDL_SCANCODE_S)) {
        cameraPos.y -= 1.f;
    }
    camera.SetPosition(cameraPos);
    float scroll {Input::system->GetState().Mouse.GetScrollValue().y};
    if (scroll != 0)
        camera.SetScale(camera.GetScale() + scroll * 0.02);

    // if (Input::system->GetState().Mouse.GetButtonState(SDL_BUTTON_LEFT) == ButtonState::Pressed) {
    //     auto& mousePos{Input::system->GetState().Mouse.GetPosition()};
    //     camera.ScreenToWorld2D(mousePos);
    // }
    // if (Input::system->GetState().Mouse.GetButtonState(SDL_BUTTON_RIGHT) == ButtonState::Pressed) {
    //     auto& mousePos{Input::system->GetState().Mouse.GetPosition()};
    //     // camera.World2DToVirtualScreen(glm::vec2{0.f});
    //     LOG_TRACE("InCamera: {}", camera.IsPointInside(camera.World2DToVirtualScreen(glm::vec2{0.f})));
    // }

    float cspeed {1.f};
    if (input.GetKeyValue(SDL_SCANCODE_LEFT)) {
        tilemapPos.x -= cspeed ; //* Time::deltaTime;
    }
    if (input.GetKeyValue(SDL_SCANCODE_RIGHT)) {
        tilemapPos.x += cspeed ; //* Time::deltaTime;
        // if (input.GetKeyState(SDL_SCANCODE_A) == ButtonState::Pressed) {
        //    tilemapPos.x -= 16.f;
    }
    if (input.GetKeyValue(SDL_SCANCODE_UP)) {
        tilemapPos.y += cspeed ; //* Time::deltaTime;
    }
    if (input.GetKeyValue(SDL_SCANCODE_DOWN)) {
        tilemapPos.y -= cspeed ; //* Time::deltaTime;
    }
    if (input.GetKeyState(SDL_SCANCODE_K) == ButtonState::Pressed) {
        DebugChangeSomeTiles();
    }
    //! ==================================================================

    Ref<Shader> shader {AssetsManager::GetShader("tilemapv3")};
    shader->Use();
    // AssetsManager::GetShader("tilemap")->SetVec2("mapSize", glm::vec2{width, height});
    shader->SetIVec2("mapSize", glm::ivec2{width, height});
    glm::mat4 model{1.0f};
    model = glm::translate(model, tilemapPos);
    shader->SetMatrix4("model", model);
    shader->SetInt("tileSize", tileSize);
    // shader->SetIVec2("atlasTexSize", glm::ivec2{16, 16});

    // TODO: Calculate this based on texture size and tileSize (a reference to a texture must be included)
    shader->SetIVec2("atlasTexSize", glm::ivec2{32, 60});

    AssetsManager::GetTexture("tilemap")->Use(0);
    vao.Use();
    vao.Draw();
}

void TilemapRenderer::DebugChangeSomeTiles() {
    std::vector<uint8_t> data(tiles.size());
    for (int i {0}; i < data.size(); ++i) {
        data[i] = Random::Range(0, 255);
    }
    vao.GetVertexBuffer().SetData(0, data.size(), data.data());
}