#include "Panel.hpp"

#include "UIStack.hpp"

#include <algorithm>

Panel::Panel(const Rect& rect) : Widget{rect} { }

Panel::Panel(const glm::vec2& size) : Widget{size} { }
