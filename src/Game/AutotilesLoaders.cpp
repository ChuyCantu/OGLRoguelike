#include "AutotilesLoaders.hpp"

#include "Rendering/Sprite.hpp"

void CreateAutotileRules4Dir(Ref<Texture> texture, const glm::ivec2& topleftCoords, int tileSize, std::unordered_map<int, TileRule>& outRules) {
    if (!outRules.empty()) outRules.clear();
    
    outRules.emplace(0,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 5, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(1,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 3, topleftCoords.y + tileSize * 2}, glm::ivec2{tileSize})}});
    outRules.emplace(2,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 4, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(3,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 0, topleftCoords.y + tileSize * 2}, glm::ivec2{tileSize})}});

    outRules.emplace(4,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 3, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(5,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 3, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(6,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 0, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(7,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 0, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});

    outRules.emplace(8,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 6, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(9,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 2, topleftCoords.y + tileSize * 2}, glm::ivec2{tileSize})}});
    outRules.emplace(10, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 5, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(11, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 1, topleftCoords.y + tileSize * 2}, glm::ivec2{tileSize})}});

    outRules.emplace(12, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 2, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(13, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 2, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(14, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 1, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(15, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 1, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
}

void CreateWallRules(Ref<class Texture> texture, const glm::ivec2& topleftCoords, int tileSize, std::unordered_map<int, TileRule>& outRules) {
    if (!outRules.empty()) outRules.clear();

    outRules.emplace(0,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 1, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(1,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 0, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(2,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 1, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(3,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 0, topleftCoords.y + tileSize * 2}, glm::ivec2{tileSize})}});

    outRules.emplace(4,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 0, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(5,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 0, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(6,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 0, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(7,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 3, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});

    outRules.emplace(8,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 1, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(9,  TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 2, topleftCoords.y + tileSize * 2}, glm::ivec2{tileSize})}});
    outRules.emplace(10, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 1, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(11, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 4, topleftCoords.y + tileSize * 2}, glm::ivec2{tileSize})}});

    outRules.emplace(12, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 2, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(13, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 5, topleftCoords.y + tileSize * 1}, glm::ivec2{tileSize})}});
    outRules.emplace(14, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 4, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
    outRules.emplace(15, TileRule{TileRule::Output::Single, {MakeRef<Sprite>(texture, glm::ivec2{topleftCoords.x + tileSize * 3, topleftCoords.y + tileSize * 0}, glm::ivec2{tileSize})}});
}