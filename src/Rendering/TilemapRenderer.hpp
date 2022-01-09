#ifndef __TILEMAPRENDERER_H__
#define __TILEMAPRENDERER_H__

#include "Common.hpp"
#include "VertexArray.hpp"
// #include "Shader.hpp"
// #include "Texture.hpp"

#include <vector>

#define ANIM_EXAMPLE

class TilemapRenderer {
public:
    TilemapRenderer(int w, int h, int tileSize);

    void Draw();

    void DebugChangeSomeTiles();

public:
    int width, height, tileSize;
    std::vector<uint16_t> tiles;

private:
    VertexArray vao;
    // Shader shader;
    // Texture texture;

#ifdef ANIM_EXAMPLE
    int activeTexture;
    std::vector<Ref<class Texture>> textures;
#endif 
};

#endif // __TILEMAPRENDERER_H__
