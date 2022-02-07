#include "UI.hpp"

#include "UIStack.hpp"

UIStack* UI::stack;

void UI::Init(UIStack* uiStack) {
    stack = uiStack;
}

Panel* UI::AddPanel(Owned<Panel> panel) {
    return stack->AddPanel(std::move(panel));
}

void UI::RemovePanel(Panel* panel) {
    stack->RemovePanel(panel);
}
