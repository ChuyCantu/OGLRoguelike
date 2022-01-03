#include "TilemapRenderer.hpp"

#include "Core/Log.hpp"
#include "Utils/Random.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

TilemapRenderer::TilemapRenderer(int w, int h, int tileSize) 
    : width{w}, height{h}, tileSize{tileSize}, tiles(w * h) {
    
    shader.Load("resources/shaders/tilemap.glsl");
    
    for (int i {0}; i < tiles.size(); ++i) {
        tiles[i] = Random::Range(0, 15);
    }

    VertexLayout layout {
        VertexElement {1, DataType::UByte, true}
    };
    vao = VertexArray{tiles.data(), static_cast<uint32_t>(tiles.size()), layout, BufferUsage::Dynamic};
    vao.SetDrawMode(DrawMode::Points);

    // texture.Load("resources/assets/TileSetSample.png", true);
    // texture.Load("resources/assets/Font.png", true);
    texture.Load("resources/assets/Custom2_16x16.png", true);
    texture.SetMinFilter(GL_NEAREST).SetMagFilter(GL_NEAREST).SetWrapS(GL_CLAMP_TO_EDGE).SetWrapT(GL_CLAMP_TO_EDGE);
}

void TilemapRenderer::Draw() 
{
    glPointSize(2.f);
    shader.Use();
    shader.SetVec2("mapSize", glm::vec2{width, height});
    shader.SetMatrix4("projection", glm::ortho(0.f, 400.f, 0.f, 300.f));
    texture.Use(0);
    vao.Use();
    vao.Draw();
}
