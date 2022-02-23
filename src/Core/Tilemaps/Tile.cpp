#include "Tile.hpp"

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

// void Tile::Refresh(const glm::ivec2& pos, uint8_t newNeighbor) {
//     if (type == TileType::Autotile) {
//         neighbors = neighbors | newNeighbor;
//     }
// }

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