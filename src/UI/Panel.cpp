#include "Panel.hpp"

#include "UIStack.hpp"

#include <algorithm>

Panel::Panel(const Rect& rect) : rect{rect} { }

Panel::Panel(const Rect& rect, size_t widgetsNum) : rect{rect}, widgets(widgetsNum) { }

Widget* Panel::AddWidget(Owned<Widget> widget) {
    SetDirty();
    widget->parentPanel = this;
    widget->SetRect(widget->GetRect());
    return widgets.emplace_back(std::move(widget)).get();
}

void Panel::RemoveWidget(Widget* widget) {
    widget->destroy = true;
    widget->parentPanel = nullptr;
    freeDestroyedWidgets = true;
}

void Panel::Update() {
    for (auto& widget : widgets) {
        if (widget->enabled)
            widget->Update();
    }
}

void Panel::RenderWidgets() {
    if (dirty) {
        std::sort(widgets.begin(), widgets.end(), 
            [](const Owned<Widget>& a, const Owned<Widget>& b) {
                return a->GetRenderOrder() < b->GetRenderOrder();
            }
        );
        dirty = false;
    }

    // glEnable(GL_SCISSOR_TEST);
    // auto screenScale {Camera::GetMainCamera().GetScreenVsVirtualSizeScaleRatio()};
    // glm::vec2 vs {Camera::GetMainCamera().GetVirtualSize() / 2};
    // vs /= screenScale;
    // glScissor(vs.x, vs.y, (int)(32.0f / screenScale.x), (int)(32.0f / screenScale.y));
    for (auto& widget : widgets) {
        if (widget->visible) {
            widget->Draw();
        }
    }
    // glDisable(GL_SCISSOR_TEST);

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

void Panel::SetRect(const Rect& rect) {
    glm::vec2 diff {rect.position - this->rect.position};
    this->rect = rect;

    for (auto& widget : widgets) {
        widget->SetRect(Rect{widget->GetRect().position + diff, widget->GetRect().size});
    }
}

void Panel::SetPosition(const glm::vec2& position) {
    glm::vec2 diff {position - rect.position};
    rect.position = position;

    for (auto& widget : widgets) {
        widget->SetRect(Rect{widget->GetRect().position + diff, widget->GetRect().size});
    }
}

void Panel::SetSize(const glm::vec2& size) {
    rect.size = size;

    for (auto& widget : widgets) {
        widget->SetRelativePosition(widget->GetRelativePivotPosition());
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
    stack->OnRenderOrderChanged();
}