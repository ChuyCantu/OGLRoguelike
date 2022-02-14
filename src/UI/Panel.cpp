#include "Panel.hpp"

#include "Rendering/Camera.hpp"
#include "UIStack.hpp"

#include <algorithm>

Panel::Panel(const std::string& name) : Widget{Rect{glm::vec2{0.0f, 0.0f}, Camera::GetMainCamera().GetVirtualSize()}, name} {
    ignoreInput = true;
}

Panel::Panel(const Rect& rect, const std::string& name) : Widget{rect, name} { 
    ignoreInput = true;
}

Panel::Panel(const glm::vec2& size, const std::string& name) : Widget{size, name} {
    ignoreInput = true;
}
