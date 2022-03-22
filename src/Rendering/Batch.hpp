#pragma once

#include "Common.hpp"
#include "Utils/Color.hpp"
#include "UI/Rect.hpp"

#include <stdint.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

void InitBatchRenderers();

//+ Sprite Batch: ===========================================================================================
struct SpriteVertex {
    glm::vec4 position;
    glm::vec2 uv;
    glm::vec4 color;
    int texIndex;
};

class SpriteBatch {
public:
    static constexpr uint32_t maxSprites  {10000};
    static constexpr uint32_t maxVertices {maxSprites * 4};
    static constexpr uint32_t maxIndices  {maxSprites * 6};
    
private:
    static std::unordered_map<Ref<class Texture>, int> textures;
    static uint32_t currentTexture;

    static std::vector<SpriteVertex> vertices;
    static uint32_t currentVertex;

    static uint32_t quadCount;

    static int maxTextureSlots;
    static bool isReadyForRendering;

public:
    static void Init();
    
    static void Start();
    static void Flush(class Shader* shader);

    static void DrawSprite(struct Transform& transform, struct SpriteRenderer& spriteRenderer, class Shader* shader);
    static void DrawSprite(const glm::mat4& model, class Sprite* sprite, Color color, const glm::vec2& size, class Shader* shader);
    static void DrawGUISprite(const Rect& rect, const glm::vec3& scale, class Sprite* sprite, Color color, const glm::mat4& model, Shader* shader);

    static bool IsReadyForRendering() { return isReadyForRendering; }
};

//+ Text Batch: ==============================================================================================
struct TextVertex {
    glm::vec4 position_uv;
    glm::vec4 color;
};

class TextBatch {
public:
    static constexpr uint32_t maxCharacters {512};
    static constexpr uint32_t maxVertices   {maxCharacters * 4};
    static constexpr uint32_t maxIndices    {maxCharacters * 6};

    static std::vector<TextVertex> vertices;
    static uint32_t currentVertex;

    static uint32_t quadCount;

public:
    static void Init();

    static void Start();
    static void Flush();
    static void Restart();

    static void AddCharacter(const TextVertex& bl, const TextVertex& br, const TextVertex& tl, const TextVertex& tr);
};