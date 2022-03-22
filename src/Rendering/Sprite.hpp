#pragma once

#include "Common.hpp"

#include <glm/vec2.hpp>

class Texture;

class Sprite {
public:
    /**
     * @brief Creates a sprite with the same size of the given texture
     * 
     * @param texture The source image for the sprite
     */
    Sprite(Ref<Texture> texture);
    
    /**
     * @brief Creates a sprite from a texture given the starting coordinates and a size of the sprite
     * 
     * @param spriteSheet The source texture (whose coordinate 0,0 is at the top-left corner)
     * @param startCoords The starting upper-left coordinates of the sprite inside the spriteSheet (in pixels)
     * @param size The sprite size in pixels
     */
    Sprite(Ref<Texture> spriteSheet, const glm::ivec2& startCoords, const glm::ivec2& size);

    const Ref<Texture> GetTexture() const { return texture; }
    const glm::ivec2& GetSize() const { return size; }
    const glm::vec2& GetMinUV() const { return spriteMinUV; }
    const glm::vec2& GetMaxUV() const { return spriteMaxUV; }

    void SetTexture(Ref<Texture> texture) { this->texture = texture; }

public:
    bool flipX{false};
    bool flipY{false};

private:
    Ref<Texture> texture;
    glm::ivec2 size;
    // Bottom-Left UV coordinate
    glm::vec2 spriteMinUV;
    // Top-Right UV coordinate
    glm::vec2 spriteMaxUV;
};