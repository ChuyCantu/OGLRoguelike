#include "Panel.hpp"

#include "UIStack.hpp"

#include <algorithm>

Panel::Panel(const Rect& rect) : Widget{rect} { 
    ignoreInput = true;
}

Panel::Panel(const glm::vec2& size) : Widget{size} {
    ignoreInput = true;
}
