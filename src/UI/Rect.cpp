#include "Rect.hpp"

bool Rect::IsPointInRect(const glm::vec2& point) {
    return point.x >= position.x && point.x <= position.x + size.x &&
           point.y >= position.y && point.y <= position.y + size.y;
}

bool Rect::Intersect(const Rect& other) {
    return !(position.x > other.position.x + other.size.x ||
             position.x + other.size.x < other.position.x ||
             position.y > other.position.y + other.size.y ||
             position.y + other.size.y < other.position.y);
}
