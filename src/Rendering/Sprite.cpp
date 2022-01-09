#include "Sprite.hpp"

#include "Texture.hpp"

Sprite::Sprite(Ref<Texture> spriteSheet, const glm::ivec2& minCoords, const glm::ivec2& maxCoords)
    : texture{spriteSheet} {
    spriteMinUV = glm::vec2{ (float)minCoords.x / (float)texture->GetWidth(), (float)minCoords.y / (float)texture->GetHeight() };
    spriteMaxUV = glm::vec2{ (float)maxCoords.x / (float)texture->GetWidth(), (float)maxCoords.y / (float)texture->GetHeight() };
}
