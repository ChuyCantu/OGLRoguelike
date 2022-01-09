#include "Sprite.hpp"

#include "Texture.hpp"

Sprite::Sprite(Ref<Texture> texture)
    : texture{texture}, size{glm::ivec2{texture->GetWidth(), texture->GetHeight()}} {
    spriteMinUV = glm::vec2 { 0.0f };
    spriteMaxUV = glm::vec2 { 1.0f };
}

Sprite::Sprite(Ref<Texture> spriteSheet, const glm::ivec2& startCoords, const glm::ivec2& size)
    : texture{spriteSheet}, size{size} {
    glm::ivec2 endCoords {startCoords + size};
    spriteMinUV = glm::vec2 { (float)startCoords.x / (float)texture->GetWidth(), (float)startCoords.y / (float)texture->GetHeight() };
    spriteMaxUV = glm::vec2 { (float)endCoords.x / (float)texture->GetWidth(), (float)endCoords.y / (float)texture->GetHeight() };
}
