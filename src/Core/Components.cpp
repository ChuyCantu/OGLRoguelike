#include "Components.hpp"

#include "Log.hpp"
#include "Rendering/Texture.hpp"
#include "GameObject.hpp"
#include "Time.hpp"
#include "Rendering/Batch.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Shader.hpp"
#include "Input/Input.hpp"
#include "Utils/Random.hpp"

#include <glm/gtc/matrix_transform.hpp>

//+ Transform =================================================================

void Transform::SetPosition(const glm::vec3& position) {
    this->position = position;
    isDirty = true;

    if (HasParent()) {
        Transform& parentTransform {gameobject->FindGameObject(parent)->GetComponent<Transform>()};
        absolutePosition = parentTransform.absolutePosition + this->position;
    }
    else {
        absolutePosition = this->position;
    }

    if (!children.empty())
        UpdateChildrenPosition();
}

void Transform::SetPosition(const glm::vec2& position) {
    this->position.x = position.x;
    this->position.y = position.y;
    this->position.z = 0.0f;
    isDirty = true;

    if (HasParent()) {
        Transform& parentTransform {gameobject->FindGameObject(parent)->GetComponent<Transform>()};
        absolutePosition = parentTransform.absolutePosition + this->position;
    } 
    else {
        absolutePosition = this->position;
    }

    if (!children.empty())
        UpdateChildrenPosition();
}

void Transform::SetAbsolutePosition(const glm::vec3& position) {
    this->position = position;
    isDirty = true;

    absolutePosition = this->position;

    if (!children.empty())
        UpdateChildrenPosition();
}

void Transform::SetAbsolutePosition(const glm::vec2& position) {
    this->position.x = position.x;
    this->position.y = position.y;
    this->position.z = 0.0f;
    isDirty = true;

    absolutePosition = this->position;

    if (!children.empty())
        UpdateChildrenPosition();
}

void Transform::SetRotation(const glm::quat& rotation) {
    this->rotation = rotation;
    isDirty = true;
}

void Transform::SetScale(const glm::vec3& scale) {
    this->scale = scale;
    isDirty = true;
}

void Transform::SetScale(const glm::vec2& scale) {
    this->scale.x = scale.x;
    this->scale.y = scale.y;
    this->scale.z = 0.0f;
    isDirty = true;
}

void Transform::AddChild(GameObject* gameobject) {
    children.emplace_back(gameobject->Entity());
    Transform& childTransform {gameobject->GetComponent<Transform>()};
    childTransform.parent = this->gameobject->Entity();
    childTransform.position = childTransform.absolutePosition - absolutePosition;
}

void Transform::RemoveChild(GameObject* gameobject) {
    auto iter {std::find(children.begin(), children.end(), gameobject->Entity())};
    if (iter != children.end()) {
        children.erase(iter);
        Transform& childTransform {gameobject->GetComponent<Transform>()};
        childTransform.parent = entt::null;
        childTransform.position = childTransform.absolutePosition;
    }
}

void Transform::RemoveChildren() {
    for (auto& childEntity : children) {
        GameObject* childGo {gameobject->FindGameObject(childEntity)};
        if (childGo) {
            Transform& childTransform {childGo->GetComponent<Transform>()};
            childTransform.parent = entt::null;
            childTransform.position = childTransform.absolutePosition;
        }
    }

    children.clear();
}

void Transform::UpdateRelativePosition() {
    SetPosition(position);
}

void Transform::UpdateChildrenPosition() {
    GameObject* child {nullptr};
    for (auto& childEntity : children) {
        child = gameobject->FindGameObject(childEntity);
        if (child)
            child->GetComponent<Transform>().UpdateRelativePosition();
    }
}

void Transform::UpdateTransform() {
    if (isDirty) {
        glm::mat4 translation {glm::translate(glm::mat4{1.0}, absolutePosition)};
        glm::mat4 rotation    {glm::mat4_cast(this->rotation)};
        glm::mat4 scale       {glm::scale(glm::mat4{1.0f}, this->scale)};
        model = translation * rotation * scale;
    }
}

GameObject* const Transform::GetParent() const { 
    return gameobject->FindGameObject(parent); 
}

//+ SpriteRenderer =================================================================
SpriteRenderer::SpriteRenderer() : sprite{MakeRef<Sprite>(AssetManager::GetTexture("missing"))} { }
SpriteRenderer::SpriteRenderer(Ref<Sprite> sprite, Color color, int renderOrder, const glm::vec2& pivot)
    : sprite{sprite}, color{color}, renderOrder{renderOrder}, pivot{pivot} { }

//+ ColliderComponent =================================================================
Collider::Collider(bool isSolid, bool ignoreSolid) : isSolid{isSolid}, ignoreSolid{ignoreSolid} { }

//+ TilemapCollider =================================================================
TilemapCollider::TilemapCollider(bool isSolid) : isSolid{isSolid} { }

//+ TilemapRenderer =================================================================

enum {
    NORTH = 0b0001,
    EAST = 0b0010,
    SOUTH = 0b0100,
    WEST = 0b1000
};

static const std::vector<glm::ivec2> directions {
    glm::ivec2{ 0,  1}, // North
    glm::ivec2{ 1,  0}, // East
    glm::ivec2{ 0, -1}, // South
    glm::ivec2{-1,  0}  // West
};

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
            // all neighbors : Refres(tilePos + dir, 0)
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
            // all neighbors : Refres(tilePos + dir, 0)
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

glm::ivec2 Tilemap::WorldSpace2TilemapSpace(int x, int y) {
    return glm::ivec2{x / tileSize, y / tileSize};
}

glm::ivec2 Tilemap::TilemapSpace2WorldSpace(int x, int y) {
    return glm::ivec2{x * tileSize, y * tileSize};
}

//+ MoveComponent =================================================================

    // TODO: Give constructor to every Component so gameobject only have one AddComponent method
    void MoveComponent::Move(glm::vec3 destination, float duration) {
        srcPosition = gameobject->GetComponent<Transform>().GetAbsolutePosition();
        destPosition = destination;
        timer = 0.f;
        time = duration;
        startedMove = true;
    }

    void MoveComponent::Teleport(glm::vec3 destination) {
        srcPosition = destination;
        destPosition = destination;
        gameobject->GetComponent<Transform>().SetPosition(destination);
    }

    void MoveComponent::Update() {
        if (timer <= time) {
            if (startedMove) startedMove = false;
            auto& transform {gameobject->GetComponent<Transform>()};
            transform.SetPosition(Lerp(srcPosition, destPosition, timer / time));
            timer += Time::deltaTime;
        }
        else {
            auto& transform{gameobject->GetComponent<Transform>()};
            transform.SetPosition(destPosition);
            srcPosition = destPosition;
            onDestinationReached.Invoke();
        }
    }

    void MoveComponent::Cancel() {
        destPosition = srcPosition;
        gameobject->GetComponent<Transform>().SetPosition(srcPosition);
        time = 0.f;
        onCancelation.Invoke();
    }