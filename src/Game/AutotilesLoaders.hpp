#pragma once

#include "Common.hpp"
#include "Core/Tilemaps/Tile.hpp"

#include <glm/vec2.hpp>

// Based on format assets/autotile4dir.png
// Number mapping based on image found in https://2dengine.com/?p=tiling
void CreateAutotileRules4Dir(Ref<class Texture> texture, const glm::ivec2& topleftCoords, int tileSize, std::unordered_map<int, TileRule>& outRules);

void CreateWallRules(Ref<class Texture> texture, const glm::ivec2& topleftCoords, int tileSize, std::unordered_map<int, TileRule>& outRules);