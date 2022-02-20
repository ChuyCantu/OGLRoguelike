#include "Components.hpp"

#include "Log.hpp"
#include "Rendering/Texture.hpp"
#include "GameObject.hpp"
#include "Time.hpp"
// #include "Rendering/VertexArray.hpp"
#include "Rendering/Batch.hpp"
#include "Rendering/Sprite.hpp"

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

//+ TilemapRenderer =================================================================
namespace std {
bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs) {
    // return lhs.x < rhs.x || lhs.x == rhs.x && (lhs.y < rhs.y || lhs.y == rhs.y);
    return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
}
}

Chunk::Chunk(int chunkSize, int tileSize) : chunkSize{chunkSize}, tileSize{tileSize}, tiles(chunkSize * chunkSize) {}

TileBase* Chunk::SetTile(int x, int y, Owned<TileBase> tile) {
    int idx {x + y * chunkSize};
    if (idx >= 0 && idx < tiles.size()) {
        tiles[idx] = std::move(tile);
        return tiles[idx].get();
    }

    return nullptr;
}

TileBase* Chunk::GetTile(int x, int y) {
    int idx {x + y * chunkSize};
    if (idx >= 0 && idx < tiles.size())
        return tiles[idx].get();

    return nullptr;
}

void Chunk::Draw(const glm::vec2& worldPos, Color color) {
    auto shader {AssetManager::GetShader("sprite").get()};
    glm::vec2 size {static_cast<float>(tileSize)};

    for (size_t y {0}; y < chunkSize; ++y) {
        for (size_t x {0}; x < chunkSize; ++x) {
            auto& currTile {tiles[x + y * chunkSize]};
            // if (!currTile) 
            //     continue;
            if (!currTile) {
                glm::mat4 tileModel{glm::translate(glm::mat4{1.0f}, glm::vec3{(worldPos.x * chunkSize + x) * tileSize, (worldPos.y * chunkSize + y) * tileSize, 0.0f})};
                Sprite missingSprite {AssetManager::GetTexture("missing")};
                SpriteBatch::DrawSprite(tileModel, &missingSprite, color, size, shader);
                continue;
            }

            glm::mat4 tileModel{glm::translate(glm::mat4{1.0f}, glm::vec3{(worldPos.x * chunkSize + x) * tileSize, (worldPos.y * chunkSize + y) * tileSize, 0.0f})};
            SpriteBatch::DrawSprite(tileModel, currTile->sprite.get(), color, size, shader);
        }
    }
}

TileBase* Tilemap::SetTile(int x, int y, Owned<TileBase> tile) {
    tile->entity = tilesRegistry.create();

    int X {x};
    int Y {y};
    if (x < 0) ++X;
    if (y < 0) ++Y;

    glm::ivec2 chunkPos;
    chunkPos.x = X / chunksSize;
    if (x < 0) --chunkPos.x;
    chunkPos.y = Y / chunksSize;
    if (y < 0) --chunkPos.y;

    // int tileX {std::abs(x % chunksSize)};
    // int tileY {std::abs(y % chunksSize)};
    int tileX {X % chunksSize};
    if (x < 0) tileX += chunksSize - 1;
    int tileY {Y % chunksSize};
    if (y < 0) tileY += chunksSize - 1;
    ASSERT(tileX < 0 || tileY < 0 || tileX >= chunksSize * chunksSize || tileY >= chunksSize * chunksSize, "Index out of bounds, tileX was {} and tileY was {}", tileX, tileY);

    // Chunk& chunk {chunks[chunkPos]};
    // return chunk.SetTile(tileX, tileY, std::move(tile));

    auto chunkIter {chunks.find(chunkPos)};
    if (chunkIter != chunks.end()) {
        return chunkIter->second.SetTile(tileX, tileY, std::move(tile));
    }
    else {
        auto result {chunks.emplace(chunkPos, Chunk{chunksSize, tileSize})};
        return result.first->second.SetTile(tileX, tileY, std::move(tile));
    }
}

TileBase* Tilemap::GetTile(int x, int y) {
    glm::ivec2 chunkPos;
    chunkPos.x = x / chunksSize;
    if (x < 0) --chunkPos.x;
    chunkPos.y = y / chunksSize;
    if (y < 0) --chunkPos.y;
    
    auto chunkIter {chunks.find(chunkPos)};
    if (chunkIter != chunks.end()) {
        int tileX {x % chunksSize};
        if (x < 0) tileX += chunksSize - 1;
        int tileY {y % chunksSize};
        if (y < 0) tileY += chunksSize - 1;

        ASSERT(tileX < 0 || tileY < 0 || tileX >= chunksSize * chunksSize || tileY >= chunksSize * chunksSize, "Index out of bounds, tileX was {} and tileY was {}", tileX, tileY);

        return chunkIter->second.GetTile(tileX, tileY);
    }

    return nullptr;
}

void Tilemap::Update() {

}

void Tilemap::Render() {
    const glm::vec2& tilemapPos {gameobject->GetComponent<Transform>().GetAbsolutePosition()};
    glm::vec2 chunkPos;
    for (auto& [vec, chunk] : chunks/*visibleChunks*/) {
        chunkPos.x = tilemapPos.x + vec.x * chunksSize;
        chunkPos.y = tilemapPos.y + vec.y * chunksSize;
        chunk.Draw(vec, color);
    }
}

glm::ivec2 Tilemap::WorldSpace2TilemapSpace(int x, int y) {
    return glm::ivec2{0};
}

glm::ivec2 Tilemap::TilemapSpace2WorldSpace(int x, int y) {
    return glm::ivec2{0};
}

//+ TilemapRendererOld ==============================================================
// TODO: Add autotiling support
TilemapRendererOld::TilemapRendererOld(GameObject* gameobject, glm::ivec2 size, int tileSize, Ref<Texture> textureAtlas, int layer) {
    this->gameobject = gameobject;
    Construct(size, tileSize, textureAtlas, layer);
}

void TilemapRendererOld::Construct(glm::ivec2 size, int tileSize, Ref<Texture> textureAtlas, int layer) {
    this->size = size;
    this->tileSize = tileSize;
    tiles = std::vector<tile_t>(size.x * size.y);
    this->textureAtlas = textureAtlas;
    atlasTexSize = glm::ivec2{textureAtlas->GetWidth() / tileSize, textureAtlas->GetHeight() / tileSize}; 
    this->layer = layer;

    VertexLayout layout { VertexElement{1, DataType::UShort, true} }; //! Datatype mush be equals to tile_t
    mesh = MakeOwned<VertexArray>(tiles.data(), static_cast<uint32_t>(tiles.size()), layout, BufferUsage::Dynamic);
    mesh->SetDrawMode(DrawMode::Points);

    tilesTypeSize = GetDataTypeSize(layout[0].dataType);
    isConstructed = true;
}

tile_t TilemapRendererOld::GetTile(int x, int y) {
    int idx {x + y * size.x};
    // if (idx >= tiles.size() || x < 0 || y < 0)
    if (x >= size.x || y >= size.y || x < 0 || y < 0)
        return 0;
    return tiles[idx];
}

void TilemapRendererOld::SetTile(int x, int y, tile_t tileIdx) {
    uint32_t idx {x + y * (uint32_t)size.x};
    // ASSERT(idx > tiles.size() - 1, "Index out of bounds. Values was: {} ({}, {}). Range was: [{} - {}]", idx, x, y, 0, tiles.size() - 1);
    ASSERT(x >= size.x || y >= size.y, "Index out of bounds. Values was: ({}, {}). Range was: ({}, {})",  x, y, size.x, size.y);
    tiles[idx] = tileIdx;

    if (uploadEndIdx == 0)
        uploadStartIdx = idx;
    else if (uploadStartIdx > idx) 
        uploadStartIdx = idx;

    if (uploadEndIdx <= idx)
        uploadEndIdx = idx + 1;
}

void TilemapRendererOld::UpdateBufferData() {
    if (uploadEndIdx > 0) {
        // int dataTypeSize {static_cast<int>(mesh->GetVertexBuffer().GetSize() / tiles.size())};
        mesh->GetVertexBuffer().SetData(uploadStartIdx * tilesTypeSize, (uploadEndIdx - uploadStartIdx) * tilesTypeSize, &tiles[uploadStartIdx]);

        uploadStartIdx = 0;
        uploadEndIdx = 0;
    }
}

// void OnTilemapAdded(entt::registry& reg, entt::entity entity) {
//     auto& tilemapRender{reg.get<TilemapRenderer>(entity)};
//     reg.emplace_or_replace<Tilemap<Tile>>(entity, tilemapRender.gameobject, tilemapRender.GetSize());
// }

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