#ifndef __BATCH_H__
#define __BATCH_H__

#include "Common.hpp"

#include <stdint.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

struct SpriteVertex {
    glm::vec4 position;
    glm::vec2 uv;
    glm::vec4 color;
    int texIndex;
};

class SpriteBatch {
public:
    static constexpr uint32_t maxSprites      {10000};
    static constexpr uint32_t maxTextureSlots {32}; // TODO: Query this from OpenGL and append a define to the shaders that use this constant
    static constexpr uint32_t maxVertices     {maxSprites * 4};
    static constexpr uint32_t maxIndices      {maxSprites * 6};

    static std::unordered_map<Ref<class Texture>, int> textures;
    static uint32_t currentTexture;

    static std::vector<SpriteVertex> vertices;
    static uint32_t currentVertex;

    static uint32_t quadCount;

    static void Init();
    
    static void Start();
    static void Flush();

    static void DrawSprite(struct Transform& transform, struct SpriteRenderer& spriteRenderer);
};

#endif // __BATCH_H__