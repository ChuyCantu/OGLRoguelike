#ifndef __RECT_H__
#define __RECT_H__

#include <glm/vec2.hpp>

struct Rect {
    glm::vec2 position {0.0f};
    glm::vec2 size     {0.0f};

    bool PointInRect(const glm::vec2& point);
    bool Intersect(const Rect& other);
};

#endif // __RECT_H__