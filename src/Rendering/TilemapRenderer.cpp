#include "TilemapRenderer.hpp"

#include "Core/AssetsManager.hpp"
#include "Input/Input.hpp"
#include "Core/Log.hpp"
#include "Rendering/Camera.hpp"
#include "Utils/Random.hpp"
#include "Core/Time.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

TilemapRenderer::TilemapRenderer(int w, int h, int tileSize) 
    : width{w}, height{h}, tileSize{tileSize}, tiles(w * h) {
    
    // shader.Load("resources/shaders/tilemap.glsl");
    AssetsManager::AddShader("tilemap", "resources/shaders/tilemap.glsl");
    
    for (int i {0}; i < tiles.size(); ++i) {
        tiles[i] = Random::Range(0, 255);
    }

    VertexLayout layout {
        VertexElement {1, DataType::UByte, true}
    };
    vao = VertexArray{tiles.data(), static_cast<uint32_t>(tiles.size()), layout, BufferUsage::Dynamic};
    vao.SetDrawMode(DrawMode::Points);

    // texture.Load("resources/assets/TileSetSample.png", true);
    // texture.Load("resources/assets/Font.png", true);
    texture.Load("resources/assets/Custom2_16x16.png", true);
    texture.SetMinFilter(TextureParameter::Nearest).SetMagFilter(TextureParameter::Nearest)
           .SetWrapS(TextureParameter::ClampToEdge).SetWrapT(TextureParameter::ClampToEdge);
}

glm::vec3 tilemapPos{0.0f};
// glm::vec3 cameraPos{0.f};
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

    if (Input::system->GetState().Mouse.GetButtonState(SDL_BUTTON_LEFT) == ButtonState::Pressed) {
        auto& mousePos{Input::system->GetState().Mouse.GetPosition()};
        camera.ScreenToWorld2D(mousePos);
    }
    if (Input::system->GetState().Mouse.GetButtonState(SDL_BUTTON_RIGHT) == ButtonState::Pressed) {
        auto& mousePos{Input::system->GetState().Mouse.GetPosition()};
        // camera.World2DToVirtualScreen(glm::vec2{0.f});
        LOG_TRACE("InCamera: {}", camera.IsPointInside(camera.World2DToVirtualScreen(glm::vec2{0.f})));
    }

    if (input.GetKeyValue(SDL_SCANCODE_LEFT)) {
        tilemapPos.x -= 10.f * Time::deltaTime;
    }
    if (input.GetKeyValue(SDL_SCANCODE_RIGHT)) {
        tilemapPos.x += 10.f * Time::deltaTime;
        // if (input.GetKeyState(SDL_SCANCODE_A) == ButtonState::Pressed) {
        //    tilemapPos.x -= 16.f;
    }
    if (input.GetKeyValue(SDL_SCANCODE_UP)) {
        tilemapPos.y += 10.f * Time::deltaTime;
    }
    if (input.GetKeyValue(SDL_SCANCODE_DOWN)) {
        tilemapPos.y -= 10.f * Time::deltaTime;
    }
    if (input.GetKeyState(SDL_SCANCODE_K) == ButtonState::Pressed) {
        DebugChangeSomeTiles();
    }
    //! ==================================================================

    AssetsManager::GetShader("tilemap").Use();
    AssetsManager::GetShader("tilemap").SetVec2("mapSize", glm::vec2{width, height});
    glm::mat4 model{1.0f};
    model = glm::translate(model, tilemapPos);
    // model = glm::rotate(model, glm::radians(30.f), glm::vec3{0.f, 0.f, 1.f});
    AssetsManager::GetShader("tilemap").SetMatrix4("model", model);

    // glm::mat4 view {glm::lookAt(cameraPos, cameraPos + glm::vec3{0.f, 0.f, -1.f}, glm::vec3{0.f, 1.f, 0.f})};
    // glm::mat4 view {1.f};
    // view = glm::translate(view, cameraPos);
    // AssetsManager::GetShader("tilemap").SetMatrix4("view", view);

    texture.Use(0);
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