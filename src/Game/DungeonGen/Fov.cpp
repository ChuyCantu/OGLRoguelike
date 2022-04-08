#include "Fov.hpp"

#include "Core/Log.hpp"
#include "PermissiveFov.hpp"

#include <glm/gtx/norm.hpp>

void FovMap::CreateMap(const glm::ivec2& size) {
    this->size = size;
    map = std::vector<FovNode>(size.x * size.y);
}

void FovMap::Compute(const glm::ivec2& origin, int radius, FovType type) {
    auto& start {GetNode(origin.x, origin.y)};
    start.visible = true;
    start.revealed = true;

    switch (type) {
        case FovType::ShadowCasting:
            for (int octant{0}; octant < 8; ++octant) {
                ShadowCastingScan(octant, origin, radius, 1, glm::ivec2{1, 1}, glm::ivec2{0, 1});
            }
            // ShadowCastingScan(4, origin, radius, 1, glm::ivec2{1, 1}, glm::ivec2{0, 1});
            break;
        case FovType::Permissive: 
            static PermissiveFov pfov;
            pfov.ComputeFov(origin, radius, *this);
            break;
    }
}

FovNode& FovMap::GetNode(int x, int y) {
    ASSERT(x < 0 || x >= size.x || y < 0 || y >= size.y || map.empty(), "Index out of bounds.");
    return map[y * size.x + x];
}

FovNode* FovMap::TryGetNode(int x, int y) {
    if (x >= 0 && x < size.x && y >= 0 && y < size.y)
        return &map[y * size.x + x];

    return nullptr;
}

void FovMap::ShadowCastingScan(int octant, const glm::ivec2& origin, int rangeLimit, int x, glm::ivec2 topSlope, glm::ivec2 bottomSlope) {
    for (; x <= rangeLimit; ++x) { // rangeLimit < 0 || x <= rangeLimit
            // compute the Y coordinates where the top vector leaves the column (on the right) and where the bottom vector
            // enters the column (on the left). this equals (x+0.5)*top+0.5 and (x-0.5)*bottom+0.5 respectively, which can
            // be computed like (x+0.5)*top+0.5 = (2(x+0.5)*top+1)/2 = ((2x+1)*top+1)/2 to avoid floating point math
            int topY {topSlope.x == 1 ? x : ((x * 2 + 1) * topSlope.y + topSlope.x - 1) / (topSlope.x * 2)};
            int bottomY {bottomSlope.y == 0 || bottomSlope.x == 0 ? 0 : ((x * 2 - 1) * bottomSlope.y + bottomSlope.x) / (bottomSlope.x * 2)};

            int wasOpaque {-1}; // 0: false, 1: true, -1: not applicable
            for (int y = topY; y >= bottomY; --y) {
                int tx {origin.x};
                int ty {origin.y};
                switch (octant) { // translate local coordinates to map coordinates
                    case 0: tx += x; ty -= y; break;
                    case 1: tx += y; ty -= x; break;
                    case 2: tx -= y; ty -= x; break;
                    case 3: tx -= x; ty -= y; break;
                    case 4: tx -= x; ty += y; break;
                    case 5: tx -= y; ty += x; break;
                    case 6: tx += y; ty += x; break;
                    case 7: tx += x; ty += y; break;
                }

                bool inRange {rangeLimit < 0 || // GetDistance(tx, ty) <= rangeLimit};
                    // x};
                    static_cast<int>(glm::distance2(glm::vec2{tx, ty}, glm::vec2{origin})) <= rangeLimit * rangeLimit};
                    // static_cast<int>(glm::distance(glm::vec2{tx, ty}, glm::vec2{origin})) <= rangeLimit};
                if (inRange) {
                    auto node {TryGetNode(tx, ty)};
                    if (node) { 
                        node->visible = true;
                        node->revealed = true;
                    }
                }

                auto* blocksLightNode {TryGetNode(tx, ty)};
                bool isOpaque {!inRange || (blocksLightNode && blocksLightNode->blocksLight) /* || BlocksLight(tx, ty)*/};
                if (x != rangeLimit) {
                    if (isOpaque) {
                        if (wasOpaque == 0) {
                            glm::ivec2 newBottom {y * 2 + 1, x * 2 - 1};
                            if (!inRange || y == bottomY) {
                                bottomSlope = newBottom;
                                break;
                            }
                            else {
                                ShadowCastingScan(octant, origin, rangeLimit, x + 1, topSlope, newBottom);
                            }
                        }
                        wasOpaque = 1;
                    }
                    else {
                        if (wasOpaque > 0) 
                            topSlope = glm::ivec2{y * 2 + 1, x * 2 + 1};
                        wasOpaque = 0;
                    }
                }
            }
            if (wasOpaque != 0) break;
        }
}