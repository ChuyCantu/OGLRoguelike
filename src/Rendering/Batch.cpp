#include "Batch.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Components.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Sprite.hpp"
#include "Utils/MathExtras.hpp"
#include "VertexArray.hpp"

#include "UI/Rect.hpp"
#include "UI/Text/TextRenderer.hpp"

void InitBatchRenderers() {
    SpriteBatch::Init();
    TextBatch::Init(); 
}

//+ Sprite Batch:
std::unordered_map<Ref<class Texture>, int> SpriteBatch::textures;
uint32_t SpriteBatch::currentTexture {0};

std::vector<SpriteVertex> SpriteBatch::vertices {maxVertices};
uint32_t SpriteBatch::currentVertex {0};

uint32_t SpriteBatch::quadCount {0};

int SpriteBatch::maxTextureSlots {32};

bool SpriteBatch::isReadyForRendering {false};

void SpriteBatch::Init() {
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureSlots);
    ASSERT(maxTextureSlots <= 0, "Failed to query Max Texture Image Units. Query returned: {}", maxTextureSlots);

    VertexLayout spriteBatchLayout{
        VertexElement{4, DataType::Float},      // Position
        VertexElement{2, DataType::Float},      // UV
        VertexElement{4, DataType::Float},      // Color
        VertexElement{1, DataType::Int, true}   // texIndex
    };

    uint32_t spriteIndices[maxIndices];
    uint32_t offset {0};
    for (size_t i {0}; i < maxIndices; i += 6) {
        spriteIndices[i]     = offset + 0;
        spriteIndices[i + 1] = offset + 1;
        spriteIndices[i + 2] = offset + 2;

        spriteIndices[i + 3] = offset + 2;
        spriteIndices[i + 4] = offset + 1;
        spriteIndices[i + 5] = offset + 3;

        offset += 4; // vertices per quad
    }

    AssetManager::AddVertexArray("spriteBatch",
                                 MakeRef<VertexArray>(nullptr, maxVertices, spriteBatchLayout, BufferUsage::Dynamic,
                                                      spriteIndices, maxIndices, BufferUsage::Static));

    //+ Shader samplers 2D set up 
    //* Sprite Shader:
    auto spriteShader {AssetManager::GetShader("sprite")};
    spriteShader->Use();
    std::vector<int> texturesShaderInit(maxTextureSlots);
    for (int i {0}; i < maxTextureSlots; ++i) {
        texturesShaderInit[i] = i;
    }
    spriteShader->SetIntv("textures", maxTextureSlots, &texturesShaderInit[0]);

    //* GUI Shader:
    auto uiShader {AssetManager::GetShader("gui")};
    uiShader->Use();
    uiShader->SetIntv("textures", maxTextureSlots, &texturesShaderInit[0]);
}

void SpriteBatch::Start() {
    currentTexture = 0;
    currentVertex = 0;
    quadCount = 0;

    textures.clear();

    isReadyForRendering = true;
}

void SpriteBatch::Flush(Shader* shader) {
    if (quadCount == 0)
        return;

    shader-> Use(); 
    auto vao {AssetManager::GetVertexArray("spriteBatch")};
    vao->Use();
    vao->GetVertexBuffer().SetData(0, quadCount * 4 * sizeof(SpriteVertex), &vertices[0]);

    for (auto& [texture, samplerID] : textures) {
        texture->Use(samplerID);
    }

    glDrawElements(GL_TRIANGLES, quadCount * 6, GL_UNSIGNED_INT, nullptr);

    isReadyForRendering = false;
}

void SpriteBatch::DrawSprite(Transform& transform, SpriteRenderer& spriteRenderer, Shader* shader) {
    if (currentVertex >= maxVertices) {
        Flush(shader);
        Start();
    }

    auto& bl {vertices[currentVertex]};
    auto& br {vertices[currentVertex + 1]};
    auto& tl {vertices[currentVertex + 2]};
    auto& tr {vertices[currentVertex + 3]};

    auto& sprite {spriteRenderer.sprite};
    glm::vec2 spriteSize {sprite->GetSize().x, sprite->GetSize().y};
    glm::vec2 pivotOffset = (spriteRenderer.pivot - glm::vec2{0.5f}) * glm::vec2{spriteSize.x, spriteSize.y};

    // float uvOffset = 0.00001;
    glm::vec2 minUV {sprite->GetMinUV().x /*+ uvOffset*/, 1.0f - sprite->GetMaxUV().y /*- uvOffset*/};
    glm::vec2 maxUV {sprite->GetMaxUV().x /*- uvOffset*/, 1.0f - sprite->GetMinUV().y /*+ uvOffset*/};

    int texIdx {0};
    auto texIter {textures.find(sprite->GetTexture())};
    if (texIter != textures.end()) {
        texIdx = texIter->second;
    }
    else {
        if (currentTexture >= maxTextureSlots) {
            Flush(shader);
            Start();
        }
        texIdx = currentTexture;
        textures.emplace(sprite->GetTexture(), currentTexture++);
    }

    auto& spriteScale {transform.GetScale()};
    float scaleX {spriteScale.x / std::abs(spriteScale.x)};
    float scaleY {spriteScale.y / std::abs(spriteScale.y)};

    bl.position = transform.GetModel() * glm::vec4{-0.5f * spriteSize.x - pivotOffset.x * scaleX, -0.5f * spriteSize.y - pivotOffset.y * scaleY, 0, 1};
    bl.uv = glm::vec2 {minUV.x, minUV.y};
    bl.color = Color2Vec4(spriteRenderer.color);
    bl.texIndex = texIdx;

    br.position = transform.GetModel() * glm::vec4{0.5f * spriteSize.x - pivotOffset.x * scaleX, -0.5f * spriteSize.y - pivotOffset.y * scaleY, 0, 1};
    br.uv = glm::vec2 {maxUV.x, minUV.y};
    br.color = Color2Vec4(spriteRenderer.color);
    br.texIndex = texIdx;

    tl.position = transform.GetModel() * glm::vec4{-0.5f * spriteSize.x - pivotOffset.x * scaleX, 0.5f * spriteSize.y - pivotOffset.y * scaleY, 0, 1};
    tl.uv = glm::vec2 {minUV.x, maxUV.y};
    tl.color = Color2Vec4(spriteRenderer.color);
    tl.texIndex = texIdx;

    tr.position = transform.GetModel() * glm::vec4{0.5f * spriteSize.x - pivotOffset.x * scaleX, 0.5f * spriteSize.y - pivotOffset.y * scaleY, 0, 1};
    tr.uv = glm::vec2 {maxUV.x, maxUV.y};
    tr.color = Color2Vec4(spriteRenderer.color);
    tr.texIndex = texIdx;

    currentVertex += 4;
    ++quadCount;
}

void SpriteBatch::DrawSprite(const glm::mat4& model, Sprite* sprite, Color color, const glm::vec2& size, Shader* shader) {
    if (currentVertex >= maxVertices) {
        Flush(shader);
        Start();
    }

    auto& bl {vertices[currentVertex]};
    auto& br {vertices[currentVertex + 1]};
    auto& tl {vertices[currentVertex + 2]};
    auto& tr {vertices[currentVertex + 3]};

    glm::vec2 spriteSize {sprite->GetSize().x, sprite->GetSize().y};
    glm::vec2 pivotOffset = (- glm::vec2{0.5f}) * glm::vec2{spriteSize.x, spriteSize.y};

    // float uvOffset = 0.00001;
    glm::vec2 minUV {sprite->GetMinUV().x /*+ uvOffset*/, 1.0f - sprite->GetMaxUV().y /*- uvOffset*/};
    glm::vec2 maxUV {sprite->GetMaxUV().x /*- uvOffset*/, 1.0f - sprite->GetMinUV().y /*+ uvOffset*/};

    int texIdx {0};
    auto texIter {textures.find(sprite->GetTexture())};
    if (texIter != textures.end()) {
        texIdx = texIter->second;
    }
    else {
        if (currentTexture >= maxTextureSlots) {
            Flush(shader);
            Start();
        }
        texIdx = currentTexture;
        textures.emplace(sprite->GetTexture(), currentTexture++);
    }

    float scaleX {size.x / std::abs(size.x)};
    float scaleY {size.y / std::abs(size.y)};

    bl.position = model * glm::vec4{-0.5f * spriteSize.x - pivotOffset.x * scaleX, -0.5f * spriteSize.y - pivotOffset.y * scaleY, 0, 1};
    bl.uv = glm::vec2 {minUV.x, minUV.y};
    bl.color = Color2Vec4(color);
    bl.texIndex = texIdx;

    br.position = model * glm::vec4{0.5f * spriteSize.x - pivotOffset.x * scaleX, -0.5f * spriteSize.y - pivotOffset.y * scaleY, 0, 1};
    br.uv = glm::vec2 {maxUV.x, minUV.y};
    br.color = Color2Vec4(color);
    br.texIndex = texIdx;

    tl.position = model * glm::vec4{-0.5f * spriteSize.x - pivotOffset.x * scaleX, 0.5f * spriteSize.y - pivotOffset.y * scaleY, 0, 1};
    tl.uv = glm::vec2 {minUV.x, maxUV.y};
    tl.color = Color2Vec4(color);
    tl.texIndex = texIdx;

    tr.position = model * glm::vec4{0.5f * spriteSize.x - pivotOffset.x * scaleX, 0.5f * spriteSize.y - pivotOffset.y * scaleY, 0, 1};
    tr.uv = glm::vec2 {maxUV.x, maxUV.y};
    tr.color = Color2Vec4(color);
    tr.texIndex = texIdx;

    currentVertex += 4;
    ++quadCount;
}

void SpriteBatch::DrawGUISprite(const Rect& rect, const glm::vec3& scale, Sprite* sprite, Color color, const glm::mat4& model, Shader* shader) {
    if (currentVertex >= maxVertices) {
        Flush(shader);
        Start();
    }

    //+ Flipped since the projection matrix used for gui is different than the one used for sprites
    auto& tl {vertices[currentVertex]};
    auto& tr {vertices[currentVertex + 1]};
    auto& bl {vertices[currentVertex + 2]};
    auto& br {vertices[currentVertex + 3]};

    glm::vec2 spriteSize {sprite->GetSize().x, sprite->GetSize().y};
    glm::vec2 pivotOffset = glm::vec2{0.5f} * glm::vec2{spriteSize.x, spriteSize.y}; 

    // float uvOffset = 0.00001;
    glm::vec2 minUV {sprite->GetMinUV().x /*+ uvOffset*/, 1.0f - sprite->GetMaxUV().y /*- uvOffset*/};
    glm::vec2 maxUV {sprite->GetMaxUV().x /*- uvOffset*/, 1.0f - sprite->GetMinUV().y /*+ uvOffset*/};

    int texIdx {0};
    auto texIter {textures.find(sprite->GetTexture())};
    if (texIter != textures.end()) {
        texIdx = texIter->second;
    }
    else {
        if (currentTexture >= maxTextureSlots) {
            Flush(shader);
            Start();
        }
        texIdx = currentTexture;
        textures.emplace(sprite->GetTexture(), currentTexture++);
    }

    float scaleX {scale.x / std::abs(scale.x)};
    float scaleY {scale.y / std::abs(scale.y)};

    // TODO: Add gui a way to flip images so the uv does not have to be inverted here

    bl.position = model * glm::vec4{0.0f, 0.0f, 0, 1};
    bl.uv = glm::vec2 {minUV.x, maxUV.y}; // glm::vec2 {minUV.x, minUV.y};
    bl.color = Color2Vec4(color);
    bl.texIndex = texIdx;

    br.position = model * glm::vec4{1.0f, 0.0f, 0, 1};
    br.uv = glm::vec2 {maxUV.x, maxUV.y}; // glm::vec2 {maxUV.x, minUV.y};
    br.color = Color2Vec4(color);
    br.texIndex = texIdx;

    tl.position = model * glm::vec4{0.0f, 1.0f, 0, 1};
    tl.uv = glm::vec2 {minUV.x, minUV.y}; // glm::vec2 {minUV.x, maxUV.y};
    tl.color = Color2Vec4(color);
    tl.texIndex = texIdx;

    tr.position = model * glm::vec4{1.0f, 1.0f, 0, 1};
    tr.uv = glm::vec2 {maxUV.x, minUV.y}; // glm::vec2 {maxUV.x, maxUV.y};
    tr.color = Color2Vec4(color);
    tr.texIndex = texIdx;

    currentVertex += 4;
    ++quadCount;
}

//+ Text Batch: ==============================================================================================
std::vector<TextVertex> TextBatch::vertices {maxCharacters};
uint32_t TextBatch::currentVertex {0};

uint32_t TextBatch::quadCount {0};

void TextBatch::Init() {
    VertexLayout textBatchLayout{
        VertexElement{4, DataType::Float}, // Position (2) - UV (2)
        VertexElement{4, DataType::Float}  // Color
    };

    uint32_t textIndices[maxIndices];
    uint32_t offset {0};
    for (size_t i {0}; i < maxIndices; i += 6) {
        textIndices[i]     = offset + 0;
        textIndices[i + 1] = offset + 1;
        textIndices[i + 2] = offset + 2;

        textIndices[i + 3] = offset + 2;
        textIndices[i + 4] = offset + 1;
        textIndices[i + 5] = offset + 3;

        offset += 4; // vertices per quad
    }

    AssetManager::AddVertexArray("textBatch",
                                 MakeRef<VertexArray>(nullptr, maxVertices, textBatchLayout, BufferUsage::Dynamic,
                                                      textIndices, maxIndices, BufferUsage::Static));
}

void TextBatch::Start() {
    currentVertex = 0;
    quadCount = 0;
}

void TextBatch::Flush() {
    if (quadCount == 0)
        return;

    auto vao {AssetManager::GetVertexArray("textBatch")};
    vao->Use();
    vao->GetVertexBuffer().SetData(0, quadCount * 4 * sizeof(SpriteVertex), &vertices[0]);

    glDrawElements(GL_TRIANGLES, quadCount * 6, GL_UNSIGNED_INT, nullptr);
}

void TextBatch::AddCharacter(const TextVertex& bl, const TextVertex& br, const TextVertex& tl, const TextVertex& tr) {
    if (currentVertex >= maxVertices) {
        Flush();
        Start();
    }

    vertices[currentVertex] = bl;
    vertices[currentVertex + 1] = br;
    vertices[currentVertex + 2] = tl;
    vertices[currentVertex + 3] = tr;

    currentVertex += 4;
    ++quadCount;
}