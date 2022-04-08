#include "Tile.hpp"

#include "Core/Log.hpp"
#include "Rendering/Batch.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Tilemap.hpp"
#include "Utils/Random.hpp"

void Tile::Start(const glm::ivec2& pos, Tilemap& tilemap) {
    this->tilemap = &tilemap;

    switch (type) {
        case TileType::Animated: {
            break;
        }
        case TileType::Random:
            if (!randomTileData->sprites.empty())
                defaultSprite = randomTileData->sprites[Random::Range(0, randomTileData->sprites.size() - 1)];  // Propably would use noise instead in the future
            break;
        case TileType::Autotile:
            for (int i {0}; i < directions.size(); ++i) {
                glm::ivec2 neighborPos {pos + directions[i]};
                Tile* neighbor {tilemap.GetTile(neighborPos.x, neighborPos.y)};

                if (neighbor && neighbor->layer == layer) {
                    switch (i) {
                        case 0:
                                neighbors |= NORTH;
                                neighbor->AddNeighbor(SOUTH);
                            break;
                        case 1:
                                neighbors |= EAST;
                                neighbor->AddNeighbor(WEST);
                            break;
                        case 2:
                                neighbors |= SOUTH;
                                neighbor->AddNeighbor(NORTH);
                            break;
                        case 3:
                                neighbors |= WEST;
                                neighbor->AddNeighbor(EAST);
                            break;
                    }
                }
            }
            break;
    }
}

void Tile::AddNeighbor(uint8_t neighbor) {
    if (type == TileType::Autotile) {
        neighbors = neighbors | neighbor;
    }
}

void Tile::RemoveNeighbor(uint8_t neighbor) {
    if (type == TileType::Autotile) {
        neighbors = neighbors ^ neighbor;
    }
}

void Tile::CopyTo(Tile* other) const {
    other->defaultSprite = defaultSprite;
    other->type = type;
    other->randomTileData = randomTileData;
    other->animatedTileData = animatedTileData;
    other->autotileData = autotileData;
    other->layer = layer;
    other->tilemap = tilemap;

    // neighbors is not copied since it is calculated when placed in the tilemap
    // entity is not copied since it is unique.
}

Sprite* Tile::GetRenderSprite() const {
    switch (type) {
        case TileType::Simple:
            return defaultSprite.get();
            break;
        case TileType::Animated:
            if (animatedTileData->sprites.empty())
                return defaultSprite.get();
            else
                return animatedTileData->sprites[tilemap->currentAnimation % animatedTileData->sprites.size()].get();
            break;
        case TileType::Random:
            return defaultSprite.get();
            break;
        case TileType::Autotile: {
            auto found {autotileData->rules.find(neighbors)};
            if (found != autotileData->rules.end()) {
                const TileRule& rule {found->second};
                ASSERT(rule.sprites.empty(), "TileRule didn't have any sprite assigned");
                if (rule.output == TileRule::Output::Single) {
                    return rule.sprites[0].get();
                }
                else if (rule.output == TileRule::Output::Animated) {
                    return rule.sprites[tilemap->currentAnimation % rule.sprites.size()].get();
                }
                
                return defaultSprite.get();
            }  
            break;
        }
    }

    return defaultSprite.get();
}

//+ Tile Assets ====================================================================

TileAsset::TileAsset(Ref<Sprite> defaultSprite, TileType type) 
    : defaultSprite{defaultSprite}, type{type} { }

SimpleTile::SimpleTile(Ref<Sprite> defaultSprite)
    : TileAsset{defaultSprite, TileType::Simple} { }

AnimatedTile::AnimatedTile(Ref<Sprite> defaultSprite, const std::vector<Ref<Sprite>>& sprites)
    : TileAsset{defaultSprite, TileType::Animated}, animatedTileData{sprites} { }

AnimatedTile::AnimatedTile(Ref<Sprite> defaultSprite, std::vector<Ref<Sprite>>&& sprites)
    : TileAsset{defaultSprite, TileType::Animated} {
    animatedTileData.sprites = std::move(sprites);
}

RandomTile::RandomTile(Ref<Sprite> defaultSprite, const std::vector<Ref<Sprite>>& sprites) 
    : TileAsset{defaultSprite, TileType::Random}, randomTileData{sprites} { }

RandomTile::RandomTile(Ref<Sprite> defaultSprite, std::vector<Ref<Sprite>>&& sprites) 
    : TileAsset{defaultSprite, TileType::Random} { 
    randomTileData.sprites = std::move(sprites);
}

AutoTile::AutoTile(Ref<Sprite> defaultSprite, const std::unordered_map<int, TileRule>& rules)
    : TileAsset{defaultSprite, TileType::Autotile}, autotileData{rules} { }

AutoTile::AutoTile(Ref<Sprite> defaultSprite, std::unordered_map<int, TileRule>&& rules)
    : TileAsset{defaultSprite, TileType::Autotile} { 
    autotileData.rules = std::move(rules);
}

Owned<Tile> SimpleTile::Instantiate() {
    Owned<Tile> tile {MakeOwned<Tile>()};

    tile->defaultSprite = defaultSprite;
    tile->type = type;
    tile->layer = layer;

    return std::move(tile);
}

Owned<Tile> AnimatedTile::Instantiate() {
    Owned<Tile> tile {MakeOwned<Tile>()};

    tile->defaultSprite = defaultSprite;
    tile->type = type;
    tile->layer = layer;
    tile->animatedTileData = &animatedTileData;

    return std::move(tile);
}

Owned<Tile> RandomTile::Instantiate() {
    Owned<Tile> tile {MakeOwned<Tile>()};

    tile->defaultSprite = defaultSprite;
    tile->type = type;
    tile->layer = layer;
    tile->randomTileData = &randomTileData;

    return std::move(tile);
}

Owned<Tile> AutoTile::Instantiate() {
    Owned<Tile> tile {MakeOwned<Tile>()};

    tile->defaultSprite = defaultSprite;
    tile->type = type;
    tile->layer = layer;
    tile->autotileData = &autotileData;
    tile->neighbors = neighbors;

    return std::move(tile);
}

//+ Tile Brush ====================================================================

TileAsset& TileBrush::CreateSimpleTile(int tileId, Ref<Sprite> defaultSprite) {
    auto& result {tiles.emplace(tileId, MakeOwned<SimpleTile>(defaultSprite))};
    LOGIF_DEBUG(!result.second, "A tile asset with the id '{}' is already registered. No insertion was done.", tileId);
    return *result.first->second.get();
}

TileAsset& TileBrush::CreateAnimatedTile(int tileId, Ref<Sprite> defaultSprite, const std::vector<Ref<Sprite>>& sprites) {
    auto& result {tiles.emplace(tileId, MakeOwned<AnimatedTile>(defaultSprite, sprites))};
    LOGIF_DEBUG(!result.second, "A tile asset with the id '{}' is already registered. No insertion was done.", tileId);
    return *result.first->second.get();
}

TileAsset& TileBrush::CreateAnimatedTile(int tileId, Ref<Sprite> defaultSprite, std::vector<Ref<Sprite>>&& sprites) {
    auto& result {tiles.emplace(tileId, MakeOwned<AnimatedTile>(defaultSprite, std::move(sprites)))};
    LOGIF_DEBUG(!result.second, "A tile asset with the id '{}' is already registered. No insertion was done.", tileId);
    return *result.first->second.get();
}

TileAsset& TileBrush::CreateRandomTile(int tileId, Ref<Sprite> defaultSprite, const std::vector<Ref<Sprite>>& sprites) {
    auto& result {tiles.emplace(tileId, MakeOwned<RandomTile>(defaultSprite, sprites))};
    LOGIF_DEBUG(!result.second, "A tile asset with the id '{}' is already registered. No insertion was done.", tileId);
    return *result.first->second.get();
}

TileAsset& TileBrush::CreateRandomTile(int tileId, Ref<Sprite> defaultSprite, std::vector<Ref<Sprite>>&& sprites) {
    auto& result {tiles.emplace(tileId, MakeOwned<RandomTile>(defaultSprite, std::move(sprites)))};
    LOGIF_DEBUG(!result.second, "A tile asset with the id '{}' is already registered. No insertion was done.", tileId);
    return *result.first->second.get();
}

TileAsset& TileBrush::CreateAutoTile(int tileId, Ref<Sprite> defaultSprite, const std::unordered_map<int, TileRule>& rules) {
    auto& result {tiles.emplace(tileId, MakeOwned<AutoTile>(defaultSprite, rules))};
    LOGIF_DEBUG(!result.second, "A tile asset with the id '{}' is already registered. No insertion was done.", tileId);
    return *result.first->second.get();
}

TileAsset& TileBrush::CreateAutoTile(int tileId, Ref<Sprite> defaultSprite, std::unordered_map<int, TileRule>&& rules) {
    auto& result {tiles.emplace(tileId, MakeOwned<AutoTile>(defaultSprite, std::move(rules)))};
    LOGIF_DEBUG(!result.second, "A tile asset with the id '{}' is already registered. No insertion was done.", tileId);
    return *result.first->second.get();
}

TileAsset* TileBrush::GetTileAsset(int tileId) {
    auto iter {tiles.find(tileId)};
    if (iter != tiles.end())
        return iter->second.get();

    return nullptr;
}

void TileBrush::DeleteTileAsset(int tileId) {
    tiles.erase(tileId);
}

Owned<Tile> TileBrush::CreateInstance(int tileId) {
    auto iter {tiles.find(tileId)};
    if (iter != tiles.end())
        return std::move(iter->second->Instantiate());

    return nullptr;
}

Tile* TileBrush::Paint(int x, int y, int tileId, Tilemap& tilemap) {
    auto tile {CreateInstance(tileId)};
    LOGIF_DEBUG(!tile, "No tile with id {} was found to be painted.", tileId);
    if (tile)
        return tilemap.SetTile(x, y, std::move(tile));
    // tilemap.SetTile(x, y, std::move(CreateInstance(tileId)));
    return nullptr;
}