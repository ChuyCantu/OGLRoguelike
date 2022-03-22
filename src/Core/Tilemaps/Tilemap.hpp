#pragma once

#include "Core/Components.hpp"
#include "Tile.hpp"

class Chunk {
public:
    Chunk(int chunkSize, int tileSize);

    Tile* SetTile(int x, int y, Owned<Tile> tile);
    Tile* GetTile(int x, int y);

    void Draw(const glm::vec2& worldPos, Color color);

    int GetChunkSize() const { return chunkSize; }

private:
    // Size per side of the chunk (so the chunk tiles count would be chunkSize * chunkSize)
    int chunkSize; 
    int tileSize;
    std::vector<Owned<Tile>> tiles;
};

namespace std {
bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs);
}

struct Tilemap : Component {
    Tilemap(int tileSize = 16, Color color = ColorNames::white, int renderOrder = 0, int chunksSize = 32);

    Tile* SetTile(int x, int y, Owned<Tile> tile);
    Tile* GetTile(int x, int y);
    
    void Update();
    void RefreshVisibleChunks(const glm::ivec2& centerChunk);
    void Render();

    void Clear();

    glm::ivec2 WorldSpace2TilemapSpace(int x, int y);
    glm::ivec2 TilemapSpace2WorldSpace(int x, int y);

public:
    int tileSize   {16};
    Color color    {ColorNames::white};
    // Chunks position is the position of the tile in the bottom-left corner of the chunk in the Tilemap space
    int renderOrder{0};
    int chunksSize {32};

    float animationsDuration {0};
    size_t currentAnimation {0};

private:
    std::map<glm::ivec2, Chunk> chunks;

    std::array<std::pair<glm::ivec2, Chunk*>, 9> visibleChunks;
    glm::ivec2 lastCameraPosition {0, 0};

    float animatorTimer {0};

    entt::registry tilesRegistry; //! Don't delete this!!! For any reason the Chunk class breaks the tiles vector if deleted...
};