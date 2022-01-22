#include "Components.hpp"

#include "Log.hpp"
#include "Rendering/Texture.hpp"
#include "GameObject.hpp"
#include "Time.hpp"
// #include "Rendering/VertexArray.hpp"

#include <glm/gtc/matrix_transform.hpp>

//+ Transform =================================================================

void Transform::SetPosition(const glm::vec3& position) {
    this->position = position;
    isDirty = true;
}

void Transform::SetPosition(const glm::vec2& position) {
    this->position.x = position.x;
    this->position.y = position.y;
    this->position.z = 0.0f;
    isDirty = true;
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

void Transform::UpdateTransform() {
    if (isDirty) {
        glm::mat4 translation {glm::translate(glm::mat4{1.0}, position)};
        glm::mat4 rotation    {glm::mat4_cast(this->rotation)};
        glm::mat4 scale       {glm::scale(glm::mat4{1.0f}, this->scale)};
        model = translation * rotation * scale;
    }
}

//+ TilemapRenderer =================================================================
// TODO: Add autotiling support
TilemapRenderer::TilemapRenderer(GameObject* gameobject, glm::ivec2 size, int tileSize, Ref<Texture> textureAtlas, int layer) {
    this->gameobject = gameobject;
    Construct(size, tileSize, textureAtlas, layer);
}

void TilemapRenderer::Construct(glm::ivec2 size, int tileSize, Ref<Texture> textureAtlas, int layer) {
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

tile_t TilemapRenderer::GetTile(int x, int y) {
    int idx {x + y * size.x};
    // if (idx >= tiles.size() || x < 0 || y < 0)
    if (x >= size.x || y >= size.y || x < 0 || y < 0)
        return 0;
    return tiles[idx];
}

void TilemapRenderer::SetTile(int x, int y, tile_t tileIdx) {
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

void TilemapRenderer::UpdateBufferData() {
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
        srcPosition = gameobject->GetComponent<Transform>().GetPosition();
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