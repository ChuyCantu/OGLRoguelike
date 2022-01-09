#include "Sprite.hpp"

#include "Texture.hpp"

Sprite::Sprite(Ref<Texture> texture)
    : texture{texture}, size{glm::ivec2{texture->GetWidth(), texture->GetHeight()}} {
    spriteMinUV = glm::vec2 { 0.0f };
    spriteMaxUV = glm::vec2 { 1.0f };
}

Sprite::Sprite(Ref<Texture> spriteSheet, const glm::ivec2& minCoords, const glm::ivec2& size)
    : texture{spriteSheet}, size{size} {
    glm::ivec2 maxCoords {minCoords + size};
    spriteMinUV = glm::vec2 { (float)minCoords.x / (float)texture->GetWidth(), (float)minCoords.y / (float)texture->GetHeight() };
    spriteMaxUV = glm::vec2 { (float)maxCoords.x / (float)texture->GetWidth(), (float)maxCoords.y / (float)texture->GetHeight() };
}
