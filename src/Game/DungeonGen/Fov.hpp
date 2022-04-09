#pragma once

#include "Common.hpp"

#include <glm/glm.hpp>

struct FovNode {
    // int lightLevel {0};
    float lightLevel {0}; // In range [0, 1]
    bool blocksLight {true};
    bool revealed {false};
    bool visible {false};
};

enum class FovType {
    ShadowCasting,
    Permissive
};

/**
 * @brief Shadow Casting FOV algorithm.
 * Reference: http://www.adammil.net/blog/v125_roguelike_vision_algorithms.html#shadowcode
 */
class FovMap {
public:
    void CreateMap(const glm::ivec2& size);
    void Compute(const glm::ivec2& origin, int radius, int fullyLitRadius, FovType type = FovType::Permissive);

    FovNode& GetNode(int x, int y);
    FovNode* TryGetNode(int x, int y);

    const glm::ivec2& GetSize() const { return size; }
    const glm::ivec2& GetMinAffectedTile() const { return minAffectedTile; }
    const glm::ivec2& GetMaxAffectedTile() const { return maxAffectedTile; }

private:
    void ShadowCastingScan(int octant, const glm::ivec2& origin, int rangeLimit, int x, glm::ivec2 topSlope, glm::ivec2 bottomSlope);
    
    void DiamondWallsScan(int octant, const glm::ivec2& origin, int rangeLimit, int x, glm::ivec2 topSlope, glm::ivec2 bottomSlope);
    bool BlocksLight(int x, int y, int octant, const glm::ivec2& origin);

public:
    // Define the square to clean before next calculation
    glm::ivec2 minAffectedTile {0, 0};
    glm::ivec2 maxAffectedTile {-1, -1};

private:
    glm::ivec2 size;
    std::vector<FovNode> map;
};