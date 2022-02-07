#ifndef __UISTACK_H__
#define __UISTACK_H__

#include "Common.hpp"
#include "Panel.hpp"

#include <vector>

// TODO: Add support for finding panels and widgets
// TODO: Make each panel render to its own texture and only render that texture if no UI element has changed
class UIStack : public Widget {
public: 
    UIStack();

    void Update() = delete;

    void HandleInput(EventHandler& eventHandler);
    void Render();

    void IterateWidgetsBackwards(Widget* widget, EventHandler& eventHandler);
};

#endif // __UISTACK_H__