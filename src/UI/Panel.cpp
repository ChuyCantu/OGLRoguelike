#include "Panel.hpp"

#include <algorithm>

Panel::Panel(const Rect& rect) : rect{rect} { }

Panel::Panel(const Rect& rect, size_t widgetsNum) : rect{rect}, widgets(widgetsNum) { }

Widget* Panel::AddWidget(Owned<Widget> widget) {
    dirty = true;
    widget->parentPanel = this;
    return widgets.emplace_back(std::move(widget)).get();
}

void Panel::RemoveWidget(Widget* widget) {
    widget->destroy = true;
    widget->parentPanel = nullptr;
    freeDestroyedWidgets = true;
}

void Panel::Update() {
    if (dirty) {
        std::sort(widgets.begin(), widgets.end(), 
            [](const Owned<Widget>& a, const Owned<Widget>& b) {
                return a->GetRenderOrder() > b->GetRenderOrder();
            }
        );
        dirty = false;
    }

    for (auto& widget : widgets) {
        if (widget->enabled)
            widget->Update();
    }

    //! Remove destroyed widgets
    if (freeDestroyedWidgets) {
        freeDestroyedWidgets = false;
        widgets.erase(std::remove_if(widgets.begin(), widgets.end(), 
            [](Owned<Widget>& w) { 
                return w->destroy;
            }
        ), widgets.end());
    }
}

void Panel::RenderWidgets() {
    for (auto& widget : widgets) {
        widget->Draw();
    }
}

// TODO: Fix SetRect/Position/Size adjustment of widgets position
void Panel::SetRect(const Rect& rect) {
    this->rect = rect;

    for (auto& widget : widgets) {
        widget->SetPosition(widget->GetRect().position + rect.position);
    }
}

void Panel::SetPosition(const glm::vec2& position) {
    rect.position = position;

    for (auto& widget : widgets) {
        widget->SetPosition(widget->GetRect().position + position);
    }
}

void Panel::SetSize(const glm::vec2& size) {
    rect.size = size;

    for (auto& widget : widgets) {
        widget->SetPosition(widget->GetRect().position);
    }
}

void Panel::SetEnabled(bool enabled) {
    if (enabled == this->enabled)
        return;

    this->enabled = enabled;
    for (auto& widget : widgets) {
        widget->enabled = enabled;
    }
}

void Panel::SetVisible(bool visible) {
    if (visible == this->visible)
        return;

    this->visible = visible;
    for (auto& widget : widgets) {
        widget->visible = visible;
    }
}

void Panel::SetDirty() {
    dirty = true;
}