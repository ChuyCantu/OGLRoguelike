#include "TilemapRenderer.hpp"

#include "Core/AssetsManager.hpp"
#include "Input/Input.hpp"
#include "Core/Log.hpp"
#include "Utils/Random.hpp"

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

glm::vec3 pos{0.0f};

void TilemapRenderer::Draw() 
{
    auto& input = Input::system->GetState().Keyboard;
    if (input.GetKeyValue(SDL_SCANCODE_D)) {
        pos.x += 1.f;
    }
    if (input.GetKeyValue(SDL_SCANCODE_A)) {
        pos.x -= 1.f;
    // if (input.GetKeyState(SDL_SCANCODE_A) == ButtonState::Pressed) {
    //    pos.x -= 16.f;
    }
    if (input.GetKeyValue(SDL_SCANCODE_W)) {
        pos.y += 1.f;
    }
    if (input.GetKeyValue(SDL_SCANCODE_S)) {
        pos.y -= 1.f;
    }
    if (input.GetKeyState(SDL_SCANCODE_K) == ButtonState::Pressed) {
        DebugChangeSomeTiles();
    }

    // glPointSize(2.f);
    // shader.Use();
    // shader.SetVec2("mapSize", glm::vec2{width, height});
    // shader.SetMatrix4("projection", glm::ortho(0.f, 320.f, 0.f, 160.f));
    AssetsManager::GetShader("tilemap").Use();
    AssetsManager::GetShader("tilemap").SetVec2("mapSize", glm::vec2{width, height});
    AssetsManager::GetShader("tilemap").SetMatrix4("projection", glm::ortho(0.f, 320.f * 2.f, 0.f, 160.f * 2.f));
    glm::mat4 model{1.0f};
    model = glm::translate(model, pos);
    AssetsManager::GetShader("tilemap").SetMatrix4("model", model);
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