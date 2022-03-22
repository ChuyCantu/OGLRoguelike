#pragma once

#include "Common.hpp"
#include "Widget.hpp"

#include <vector>

class UIStack;

// TODO: Make Panel a Widget and make all widgets be able to have children 
class Panel : public Widget {
public:
    // Initialize a panel at position (0, 0) and size equals to Camera virtual size
    Panel(const std::string& name = "Panel"); 
    Panel(const Rect& rect, const std::string& name = "Panel");
    Panel(const glm::vec2& size, const std::string& name = "Panel");

    void Draw() override { }
    void HandleInput(EventHandler& eventHandler) override { }

private: 
    // TODO: Check when widget sortOrder change and make this dirty too!!!
    
    UIStack* stack;

    friend class UIStack;
};