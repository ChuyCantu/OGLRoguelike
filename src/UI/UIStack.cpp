#include "UIStack.hpp"

#include <algorithm>

UIStack::UIStack() : Widget{} { 
    parent = nullptr; // Root of all widgets
    ignoreInput = true;
}

void UIStack::HandleInput(EventHandler& eventHandler) {
    SortChildren();

    IterateWidgetsBackwards(this, eventHandler);

    RemovePendingChildren();
}

void UIStack::Render() {
    SortChildren();

    for (auto& panel : children) {
        if (panel->IsVisible())
            panel->Render();
    }

    RemovePendingChildren();
}

void UIStack::IterateWidgetsBackwards(Widget* widget, EventHandler& eventHandler) {
    for (auto child {widget->GetChildren().rbegin()}; child != widget->GetChildren().rend(); ++child) {
        if ((*child)->IsVisible() && (*child)->IsEnabled()) {
            if ((*child)->childrenIgnoreInput)
                continue;

            IterateWidgetsBackwards((*child).get(), eventHandler);

            if (!(*child)->ignoreInput)
                (*child)->HandleInput(eventHandler);
            
            if (eventHandler.handled)
                return;
        }
    }
}
