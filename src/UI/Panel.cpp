#include "Panel.hpp"

#include "Rendering/Camera.hpp"
#include "UIStack.hpp"

#include <algorithm>

Panel::Panel() : Widget{Rect{glm::vec2{0.0f, 0.0f}, Camera::GetMainCamera().GetVirtualSize()}} {
    ignoreInput = true;
}

Panel::Panel(const Rect& rect) : Widget{rect} { 
    ignoreInput = true;
}

Panel::Panel(const glm::vec2& size) : Widget{size} {
    ignoreInput = true;
}
