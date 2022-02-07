#ifndef __PANEL_H__
#define __PANEL_H__

#include "Common.hpp"
#include "Widget.hpp"

#include <vector>

class UIStack;

// TODO: Make Panel a Widget and make all widgets be able to have children 
class Panel : public Widget {
public:
    Panel(const Rect& rect);
    Panel(const glm::vec2& size);

    void Draw() override { }
    void HandleInput() override { }

private: 
    // TODO: Check when widget sortOrder change and make this dirty too!!!
    
    UIStack* stack;

    friend class UIStack;
};

#endif // __PANEL_H__