#include "UI.hpp"

#include "UIStack.hpp"

Widget* UI::focused;
Widget* UI::hovered;
UIStack* UI::stack;

void UI::Init(UIStack* uiStack) {
    stack = uiStack;
}

Widget* UI::AddPanel(Owned<Panel> panel) {
    return stack->AddChild(std::move(panel));
}

void UI::RemovePanel(Panel* panel) {
    stack->RemoveChild(panel);
}
