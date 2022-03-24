#include "Tilemap.hpp"

#include "Input/Input.hpp"
#include "Rendering/Batch.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Core/Time.hpp"
#include "Utils/Random.hpp"

namespace std {
bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs) {
    // return lhs.x < rhs.x || lhs.x == rhs.x && (lhs.y < rhs.y || lhs.y == rhs.y);
    return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
}
}

Chunk::Chunk(int chunkSize, int tileSize) : chunkSize{chunkSize}, tileSize{tileSize}, tiles(chunkSize * chunkSize) {}

Tile* Chunk::SetTile(int x, int y, Owned<Tile> tile) {
    int idx {x + y * chunkSize};
    if (idx >= 0 && idx < tiles.size()) {
        tiles[idx] = std::move(tile);
        return tiles[idx].get();
    }

    return nullptr;
}

Tile* Chunk::GetTile(int x, int y) {
    int idx {x + y * chunkSize};
    if (idx >= 0 && idx < tiles.size())
        return tiles[idx].get();

    return nullptr;
}

void Chunk::Draw(const glm::vec2& worldPos, Color color) {
    Shader* shader {AssetManager::GetShader("sprite").get()};
    glm::vec2 size {static_cast<float>(tileSize)};

    for (size_t y {0}; y < chunkSize; ++y) {
        for (size_t x {0}; x < chunkSize; ++x) {
            auto& currTile {tiles[x + y * chunkSize]};
            if (!currTile || !currTile->defaultSprite) 
                continue;
            // if (!currTile || !currTile->sprite) {
            //     glm::mat4 tileModel{glm::translate(glm::mat4{1.0f}, glm::vec3{(worldPos.x * chunkSize + x) * tileSize, (worldPos.y * chunkSize + y) * tileSize, 0.0f})};
            //     Sprite missingSprite {AssetManager::GetTexture("missing")};
            //     SpriteBatch::DrawSprite(tileModel, &missingSprite, color, size, shader);
            //     continue;
            // }

            // TODO: Figure out better way for this (cache the vertex position on each tile since it will never change):
            glm::mat4 tileModel{glm::translate(glm::mat4{1.0f}, glm::vec3{(worldPos.x * chunkSize + x) * tileSize, (worldPos.y * chunkSize + y) * tileSize, 0.0f})};
            SpriteBatch::DrawSprite(tileModel, currTile->GetRenderSprite(), color, size, shader);
        }
    }
}

Tilemap::Tilemap(int tileSize, Color color, int renderOrder, int chunksSize) : tileSize{tileSize}, color{color}, renderOrder{renderOrder}, chunksSize{chunksSize} {
    visibleChunks[0].first = glm::ivec2{-1, -1}; // Bottom-Left
    visibleChunks[1].first = glm::ivec2{ 0, -1}; // Bottom
    visibleChunks[2].first = glm::ivec2{ 1, -1}; // Bottom-Right
    visibleChunks[3].first = glm::ivec2{-1,  0}; // Left
    visibleChunks[4].first = glm::ivec2{ 0,  0}; // Center
    visibleChunks[5].first = glm::ivec2{ 1,  0}; // Right
    visibleChunks[6].first = glm::ivec2{-1,  1}; // Top-Left
    visibleChunks[7].first = glm::ivec2{ 0,  1}; // Top
    visibleChunks[8].first = glm::ivec2{ 1,  1}; // Right
}

Tile* Tilemap::SetTile(int x, int y, Owned<Tile> tile) {
    int X {x};
    int Y {y};
    if (x < 0) ++X;
    if (y < 0) ++Y;

    glm::ivec2 chunkPos;
    chunkPos.x = X / chunksSize;
    if (x < 0) --chunkPos.x;
    chunkPos.y = Y / chunksSize;
    if (y < 0) --chunkPos.y;

    glm::ivec2 tilePos;
    tilePos.x = X % chunksSize;
    if (x < 0) tilePos.x += chunksSize - 1;
    tilePos.y = Y % chunksSize;
    if (y < 0) tilePos.y += chunksSize - 1;
    ASSERT(tilePos.x < 0 || tilePos.y < 0 || tilePos.x >= chunksSize * chunksSize || tilePos.y >= chunksSize * chunksSize, "Index out of bounds, tileX was {} and tileY was {}", tilePos.x, tilePos.y);

    auto chunkIter {chunks.find(chunkPos)};
    if (chunkIter != chunks.end()) {
        // Check if this chunk is visible (without iterating all 9 chunks each time)
        if (chunkPos.x >= visibleChunks[0].first.x && chunkPos.y >= visibleChunks[0].first.y && chunkPos.x <= visibleChunks[8].first.x && chunkPos.y <= visibleChunks[8].first.y) {
            int idX {chunkPos.x - visibleChunks[0].first.x};
            int idY {chunkPos.y - visibleChunks[0].first.y};
            int idx {idX + idY * 3};
            if (visibleChunks[idx].second == nullptr)
                visibleChunks[idx].second = &chunkIter->second;
        }

        if (tile)
            tile->Start({x, y}, *this);
        else {
            // all neighbors : Refresh(tilePos + dir, 0)
            Tile* prevTile {GetTile(x, y)};
            if (prevTile) {
                for (int i {0}; i < directions.size(); ++i) {
                    glm::ivec2 neighborPos {glm::ivec2{x, y} + directions[i]};
                    Tile* neighbor {GetTile(neighborPos.x, neighborPos.y)};

                    if (neighbor && neighbor->layer == prevTile->layer) {
                        switch (i) {
                            case 0:
                                    neighbor->RemoveNeighbor(SOUTH);
                                break;
                            case 1:
                                    neighbor->RemoveNeighbor(WEST);
                                break;
                            case 2:
                                    neighbor->RemoveNeighbor(NORTH);
                                break;
                            case 3:
                                    neighbor->RemoveNeighbor(EAST);
                                break;
                        }
                    }
                }
            }
        }

        return chunkIter->second.SetTile(tilePos.x, tilePos.y, std::move(tile));
    }
    else {
        if (tile)
            tile->Start({x, y}, *this);
        else {
            // all neighbors : Refresh(tilePos + dir, 0)
            Tile* prevTile{GetTile(x, y)};
            if (prevTile) {
                for (int i{0}; i < directions.size(); ++i) {
                    glm::ivec2 neighborPos{glm::ivec2{x, y} + directions[i]};
                    Tile* neighbor{GetTile(neighborPos.x, neighborPos.y)};

                    if (neighbor && neighbor->layer == prevTile->layer) {
                        switch (i) {
                            case 0:
                                neighbor->RemoveNeighbor(SOUTH);
                                break;
                            case 1:
                                neighbor->RemoveNeighbor(WEST);
                                break;
                            case 2:
                                neighbor->RemoveNeighbor(NORTH);
                                break;
                            case 3:
                                neighbor->RemoveNeighbor(EAST);
                                break;
                        }
                    }
                }
            }
           }

        auto result {chunks.emplace(chunkPos, Chunk{chunksSize, tileSize})};
        return result.first->second.SetTile(tilePos.x, tilePos.y, std::move(tile));
    }
}

Tile* Tilemap::GetTile(int x, int y) {
    int X{x};
    int Y{y};
    if (x < 0) ++X;
    if (y < 0) ++Y;

    glm::ivec2 chunkPos;
    chunkPos.x = X / chunksSize;
    if (x < 0) --chunkPos.x;
    chunkPos.y = Y / chunksSize;
    if (y < 0) --chunkPos.y;

    auto chunkIter {chunks.find(chunkPos)};
    if (chunkIter != chunks.end()) {
        int tileX {X % chunksSize};
        if (x < 0) tileX += chunksSize - 1;
        int tileY {Y % chunksSize};
        if (y < 0) tileY += chunksSize - 1;

        ASSERT(tileX < 0 || tileY < 0 || tileX >= chunksSize * chunksSize || tileY >= chunksSize * chunksSize, "Index out of bounds, tileX was {} and tileY was {}", tileX, tileY);

        return chunkIter->second.GetTile(tileX, tileY);
    }

    return nullptr;
}

void Tilemap::Update() {
    // Find what chunks should be rendered
    glm::ivec2 cameraPos {Camera::GetMainCamera().GetPosition()};
    cameraPos /= tileSize;

    // Find what chunk the camera is inside of
    int X{cameraPos.x};
    int Y{cameraPos.y};
    if (cameraPos.x < 0) ++X;
    if (cameraPos.y < 0) ++Y;

    glm::ivec2 chunkPos;
    chunkPos.x = X / chunksSize;
    if (cameraPos.x < 0) --chunkPos.x;
    chunkPos.y = Y / chunksSize;
    if (cameraPos.y < 0) --chunkPos.y;

    // Fill necessary chunks if moved
    if (chunkPos != visibleChunks[4].first) {
        RefreshVisibleChunks(chunkPos);
    }

    // Update animation
    if (animationsDuration != 0) {
        animatorTimer += Time::deltaTime;
        if (animatorTimer >= animationsDuration) {
            animatorTimer -= animationsDuration;

            ++currentAnimation;
        }
    }   
}

void Tilemap::RefreshVisibleChunks(const glm::ivec2& centerChunk) {
    int idx {0};
    for (int y {-1}; y <= 1; ++y) {
        for (int x {-1}; x <= 1; ++x) {
            glm::ivec2 chunkPos {centerChunk + glm::ivec2{x, y}};
            visibleChunks[idx].first = chunkPos;

            auto chunkIter {chunks.find(chunkPos)};
            if (chunkIter != chunks.end()) 
                visibleChunks[idx].second = &chunkIter->second;
            else
                visibleChunks[idx].second = nullptr;

            ++idx;
        }
    }
}

void Tilemap::Render() {
    glm::vec2 chunkPos;
    for (auto& [vec, chunk] : visibleChunks) {
        if (!chunk)
            continue;

        chunkPos.x = vec.x * chunksSize;
        chunkPos.y = vec.y * chunksSize;
        chunk->Draw(vec, color);
    }
    // for (auto& [vec, chunk] : chunks) {
    //     chunkPos.x = vec.x * chunksSize;
    //     chunkPos.y = vec.y * chunksSize;
    //     chunk.Draw(vec, color);
    // }
}

void Tilemap::Clear() {
    chunks.clear();

    visibleChunks[0].first = glm::ivec2{-1, -1}; // Bottom-Left
    visibleChunks[1].first = glm::ivec2{ 0, -1}; // Bottom
    visibleChunks[2].first = glm::ivec2{ 1, -1}; // Bottom-Right
    visibleChunks[3].first = glm::ivec2{-1,  0}; // Left
    visibleChunks[4].first = glm::ivec2{ 0,  0}; // Center
    visibleChunks[5].first = glm::ivec2{ 1,  0}; // Right
    visibleChunks[6].first = glm::ivec2{-1,  1}; // Top-Left
    visibleChunks[7].first = glm::ivec2{ 0,  1}; // Top
    visibleChunks[8].first = glm::ivec2{ 1,  1}; // Right

    for (auto& chunk : visibleChunks) {
        chunk.second = nullptr;
    }

    currentAnimation = 0;
    animatorTimer = 0;
}

glm::ivec2 Tilemap::WorldSpace2TilemapSpace(int x, int y) {
    return glm::ivec2{x / tileSize, y / tileSize};
}

glm::ivec2 Tilemap::TilemapSpace2WorldSpace(int x, int y) {
    return glm::ivec2{x * tileSize, y * tileSize};
}