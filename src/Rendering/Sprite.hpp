#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "Common.hpp"

#include <glm/vec2.hpp>

class Texture;

class Sprite {
public:
    Sprite(Ref<Texture> texture);
    Sprite(Ref<Texture> spriteSheet, const glm::ivec2& minCoords, const glm::ivec2& size);

    const Ref<Texture> GetTexture() const { return texture; }

    const glm::ivec2& GetSize() const { return size; }
    const glm::vec2& GetMinUV() const { return spriteMinUV; };
    const glm::vec2& GetMaxUV() const { return spriteMaxUV; };

private:
    Ref<Texture> texture;
    glm::ivec2 size;
    glm::vec2 spriteMinUV;
    glm::vec2 spriteMaxUV;
};

#endif // __SPRITE_H__