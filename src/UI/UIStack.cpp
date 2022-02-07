#include "UIStack.hpp"

#include <algorithm>

Panel* UIStack::AddPanel(Owned<Panel> panel) {
    needReordering = true;
    panel->stack = this;
    return panels.emplace_back(std::move(panel)).get();
}

void UIStack::RemovePanel(Panel* panel) {
    auto found {std::find_if(panels.begin(), panels.end(), [&](const Owned<Panel>& a) {
        return a.get() == panel;
    })};
    if (found != panels.end())
        panels.erase(found);
}

void UIStack::Update() {
    if (needReordering) {
        std::sort(panels.begin(), panels.end(), 
            [](const Owned<Panel>& a, const Owned<Panel>& b) {
                return a->GetRenderOrder() < b->GetRenderOrder();
            }
        );
        needReordering = false;
    }

    IteratePanels();
}

void UIStack::RenderPanels() {
    if (needReordering) {
        std::sort(panels.begin(), panels.end(), 
            [](const Owned<Panel>& a, const Owned<Panel>& b) {
                return a->GetRenderOrder() < b->GetRenderOrder();
            }
        );
        needReordering = false;
    }

    for (auto& panel : panels) {
        if (panel->visible)
            panel->Render();
    }
}

void IterateWidgetsBackwards(Widget* widget) {
    for (auto child {widget->GetChildren().rbegin()}; child != widget->GetChildren().rend(); ++child) {
        IterateWidgetsBackwards((*child).get());

        (*child)->Update();
    }
}

void UIStack::IteratePanels() {
    for (auto panel {panels.rbegin()}; panel != panels.rend(); ++panel) {
        IterateWidgetsBackwards((*panel).get());
    }
}

void UIStack::OnRenderOrderChanged() {
    needReordering = true;
}
