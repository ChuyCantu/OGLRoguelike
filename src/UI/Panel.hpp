#ifndef __PANEL_H__
#define __PANEL_H__

#include "Common.hpp"
#include "Widget.hpp"

#include <vector>

class UIStack;

// TODO: Make Panel a Widget and make all widgets be able to have children 
class Panel {
public:
    Panel(const Rect& rect);
    Panel(const Rect& rect, size_t widgetsNum);
    
    Widget* AddWidget(Owned<Widget> widget);
    void RemoveWidget(Widget* widget);

    void Update();
    void RenderWidgets();

    void SetRect(const Rect& rect);
    void SetPosition(const glm::vec2& position);
    void SetSize(const glm::vec2& size);
    void SetEnabled(bool enabled);
    void SetVisible(bool visible);

    const Rect& GetRect() const { return rect; }
    const auto& GetWidgets() const { return widgets; }
    bool IsEnabled() const { return enabled; }
    bool IsVisible() const { return visible; }

    // This should be called only by the children widgets
    void SetDirty();

public:
    int renderOrder {0};

private: 
    Rect rect;
    std::vector<Owned<class Widget>> widgets;
    // TODO: Check when widget sortOrder change and make this dirty too!!!
    bool dirty                {true};
    bool freeDestroyedWidgets {false};
    bool enabled              {true};
    bool visible              {true};
    UIStack* stack;

    friend class TestScene;
    friend class Scene;
    friend class UIStack;
};

#endif // __PANEL_H__