#ifndef __TILE_H__
#define __TILE_H__

#include "Common.hpp"

#include <glm/vec2.hpp>

class Sprite;
struct Tilemap;

struct TileRule {
    enum class Output {
        Single,
        Animated
    } output{Output::Single};
    std::vector<Ref<Sprite>> sprites;
};

struct AutotileData {
    std::unordered_map<int, TileRule> rules;
};

struct AnimatedTileData {
    std::vector<Ref<Sprite>> sprites;
};

struct RandomTileData {
    std::vector<Ref<Sprite>> sprites;
};

enum class TileType { 
    Simple,   // Display a single sprite
    Animated, // Display a sprite animation given the sprite frames
    Random,   // Display a random sprite from the list given
    Autotile    
};

enum {
    NORTH = 0b0001,
    EAST  = 0b0010,
    SOUTH = 0b0100,
    WEST  = 0b1000
};

static const std::vector<glm::ivec2> directions {
    glm::ivec2{ 0,  1}, // North
    glm::ivec2{ 1,  0}, // East
    glm::ivec2{ 0, -1}, // South
    glm::ivec2{-1,  0}  // West
};

class Tile {
public:
    void Start(const glm::ivec2& pos, Tilemap& tilemap);
    // void Refresh(const glm::ivec2& pos, uint8_t newNeighbor);
    void AddNeighbor(uint8_t neighbor);
    void RemoveNeighbor(uint8_t neighbor);

    // Copies data from one Tile to another. This must be used before sending the tile into the tilemap since it may behave wrong if the copied tile is an autotile
    void CopyTo(Tile* other) const;

    Sprite* GetRenderSprite() const;

public:
    // Used for all types of tiles
    Ref<Sprite> defaultSprite {nullptr};
    
    TileType type {TileType::Simple};
    
    const RandomTileData* randomTileData     {nullptr};
    const AnimatedTileData* animatedTileData {nullptr};
    const AutotileData* autotileData         {nullptr};
    uint8_t neighbors {0};
    int layer {0}; // Used for autotiled tiles. If the layer on a neighbour tile is the same as the autotiled tile, it will be considered for the tile bitmask

private:
    Tilemap* tilemap {nullptr};
};

struct TileAsset {
    Ref<Sprite> defaultSprite;
    TileType type;
    int layer {0};

    virtual Owned<Tile> Instantiate() = 0;

protected:
    TileAsset(Ref<Sprite> defaultSprite, TileType type);
};

struct SimpleTile : TileAsset {
    SimpleTile(Ref<Sprite> defaultSprite);

    Owned<Tile> Instantiate() override;
};

struct AnimatedTile : TileAsset {
    AnimatedTile(Ref<Sprite> defaultSprite, const std::vector<Ref<Sprite>>& sprites);
    AnimatedTile(Ref<Sprite> defaultSprite, std::vector<Ref<Sprite>>&& sprites);

    AnimatedTileData animatedTileData;

    Owned<Tile> Instantiate() override;
};

struct RandomTile : TileAsset {
    RandomTile(Ref<Sprite> defaultSprite, const std::vector<Ref<Sprite>>& sprites);
    RandomTile(Ref<Sprite> defaultSprite, std::vector<Ref<Sprite>>&& sprites);

    RandomTileData randomTileData;

    Owned<Tile> Instantiate() override;
};

struct AutoTile : TileAsset {
    AutoTile(Ref<Sprite> defaultSprite, const std::unordered_map<int, TileRule>& rules);
    AutoTile(Ref<Sprite> defaultSprite, std::unordered_map<int, TileRule>&& rules);

    AutotileData autotileData;
    uint8_t neighbors{0};

    Owned<Tile> Instantiate() override;
};

class TileBrush {
public:
    TileAsset& CreateSimpleTile(const std::string& tileId, Ref<Sprite> defaultSprite);
    TileAsset& CreateAnimatedTile(const std::string& tileId, Ref<Sprite> defaultSprite, const std::vector<Ref<Sprite>>& sprites);
    TileAsset& CreateAnimatedTile(const std::string& tileId, Ref<Sprite> defaultSprite, std::vector<Ref<Sprite>>&& sprites);
    TileAsset& CreateRandomTile(const std::string& tileId, Ref<Sprite> defaultSprite, const std::vector<Ref<Sprite>>& sprites);
    TileAsset& CreateRandomTile(const std::string& tileId, Ref<Sprite> defaultSprite, std::vector<Ref<Sprite>>&& sprites);
    TileAsset& CreateAutoTile(const std::string& tileId, Ref<Sprite> defaultSprite, const std::unordered_map<int, TileRule>& rules);
    TileAsset& CreateAutoTile(const std::string& tileId, Ref<Sprite> defaultSprite, std::unordered_map<int, TileRule>&& rules);

    TileAsset* GetTileAsset(const std::string& tileId);
    void DeleteTileAsset(const std::string& tileId);
    Owned<Tile> CreateInstance(const std::string& tileId);

    void Paint(int x, int y, const std::string& tileId, Tilemap& tilemap);

private:
    std::unordered_map<std::string, Owned<TileAsset>> tiles;
};

#endif // __TILE_H__