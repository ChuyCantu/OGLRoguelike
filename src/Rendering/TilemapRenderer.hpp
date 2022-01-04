#ifndef __TILEMAPRENDERER_H__
#define __TILEMAPRENDERER_H__

#include "VertexArray.hpp"
// #include "Shader.hpp"
#include "Texture.hpp"

#include <vector>

class TilemapRenderer {
public:
    TilemapRenderer(int w, int h, int tileSize);

    void Draw();

    void DebugChangeSomeTiles();

public:
    int width, height, tileSize;
    std::vector<uint8_t> tiles;

private:
    VertexArray vao;
    // Shader shader;
    Texture texture;
};

#endif // __TILEMAPRENDERER_H__

//+ Tilemap
// public readonly int Width, Height;
// public readonly byte[] Tiles;
// public int TileSetHandle { get; set; }

// /*
//  * This multiplication-based index calculation can be avoided if the width of
//  * the map is a power of 2. In that case you can use bit-shifting for efficiency.
//  */
// public ref byte this[int x, int y] => ref Tiles[x + y * Width];

// public TileMap(int width, int height)
// {
//     if (width <= 0) throw new ArgumentOutOfRangeException(nameof(width), "Must be positive.");
//     if (height <= 0) throw new ArgumentOutOfRangeException(nameof(height), "Must be positive.");

//     Width = width;
//     Height = height;
//     Tiles = new byte[width * height];
// }

//+ Renderer
// public const int TileSize = 32;
// public const float TileTexSize = 1 / 16f;
// public const float TileTexPadding = 1 / 256f;
// Vector2 Center { get; set; }
// TileMap TileMap { get; }

// void Initialize(TileMap tileMap);
        
// void Render();

// void OnBackBufferResized(int width, int height);