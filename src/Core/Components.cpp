#include "Components.hpp"

#include "Log.hpp"
#include "Rendering/Texture.hpp"
// #include "Rendering/VertexArray.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Transform::SetPosition(const glm::vec3& position) {
    this->position = position;
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

void Transform::UpdateTransform() {
    if (isDirty) {
        // model = glm::translate(model, position);
        // model = model * glm::mat4_cast(rotation);
        // model = glm::scale(model, scale);
        glm::mat4 translation {glm::translate(glm::mat4{1.0}, position)};
        glm::mat4 rotation    {glm::mat4_cast(this->rotation)};
        glm::mat4 scale       {glm::scale(glm::mat4{1.0f}, this->scale)};
        model = translation * rotation * scale;
    }
}

void TilemapRenderer::Construct(glm::ivec2 size, int tileSize, Ref<Texture> textureAtlas, int layer) {
    this->size = size;
    this->tileSize = tileSize;
    tiles = std::vector<tile_t>(size.x * size.y);
    this->textureAtlas = textureAtlas;
    atlasTexSize = glm::ivec2{textureAtlas->GetWidth() / tileSize, textureAtlas->GetHeight() / tileSize};  // glm::ivec2{8, 32};
    this->layer = layer;

    for (int i{0}; i < tiles.size(); ++i) {
        tiles[i] = 209; 
    }

    VertexLayout layout { VertexElement{1, DataType::UShort, true} }; //! Datatype mush be equals to tile_t
    mesh = MakeOwned<VertexArray>(tiles.data(), static_cast<uint32_t>(tiles.size()), layout, BufferUsage::Dynamic);
    mesh->SetDrawMode(DrawMode::Points);

    tilesTypeSize = GetDataTypeSize(layout[0].dataType);
    isConstructed = true;
}

VertexArray* TilemapRenderer::GetMesh() { return mesh.get(); }

void TilemapRenderer::SetTile(int x, int y, tile_t tileIdx) {
    uint32_t idx {x + y * (uint32_t)size.x};
    LOGIF_CRITICAL(idx > tiles.size(), "Index out of bounds. Values was: {} ({}, {}). Range was: [{} - {}]", idx, x, y, 0, tiles.size() - 1);
    assert(idx < tiles.size());
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
        // TODO: Optimize this
        // int dataTypeSize {static_cast<int>(mesh->GetVertexBuffer().GetSize() / tiles.size())};
        mesh->GetVertexBuffer().SetData(uploadStartIdx * tilesTypeSize, (uploadEndIdx - uploadStartIdx) * tilesTypeSize, &tiles[uploadStartIdx]);

        // auto ptr {(tile_t*)mesh->GetVertexBuffer().Map(BufferAccess::WriteOnly)};
        // for (int i {1}; i < tiles.size() - 2; ++i)
        //     ptr[i] = tiles[i];
        // mesh->GetVertexBuffer().Unmap();

        uploadStartIdx = 0;
        uploadEndIdx = 0;
    }
}