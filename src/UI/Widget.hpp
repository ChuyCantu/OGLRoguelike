#ifndef __WIDGET_H__
#define __WIDGET_H__

#include "Common.hpp"
#include "Core/Event.hpp"
#include "Rect.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

enum class Anchor {
    TopLeft,
    TopRight,
    Center,
    BottomLeft,
    BottomRight
};

class Widget {
public:
    Widget();
    Widget(const Rect& rect);
    Widget(const glm::vec2& size);
    virtual ~Widget();

    // Moves the widget to an absolute position on screen
    void SetAbsolutePosition(const glm::vec2& position);
    // Moves the widget relative to its parent panel
    void SetPosition(const glm::vec2& position);
    void SetSize(const glm::vec2& size);
    void SetRect(const Rect& rect);
    void SetPivot(const glm::vec2& pivot);
    void SetAnchor(Anchor anchor);
    void SetRenderOrder(int value);

    void SetEnabled(bool enabled);
    void SetVisible(bool visible);

    virtual void Update(); // TODO: Input handling
    void Render();

    Widget* AddChild(Owned<Widget> child);
    void RemoveChild(Widget* child);
    void RemoveAllChildren();
    // Widget* GetChildren();
    void UpdateChildrenPositions();
    void UpdateRelativePosition();

    void UpdateTransform();

    // Sets the position and size of the widget as if the anchor was set to top left
    // adjusting the pivot and anchor values of the widget
    const Rect& GetRect() const { return rect; }
    const glm::vec2& GetSize() const { return rect.size; }
    const Anchor GetAnchor() const { return anchor; }
    const glm::vec2& GetPivot() const { return pivot; }
    const glm::vec2& GetAbsolutePivotPosition() const { return absolutePivotPosition; }
    const glm::vec2& GetPosition() const { return position; }
    Widget* GetParent() { return parent; }
    int GetRenderOrder() const { return renderOrder; };
    bool IsEnabled() const { return enabled; }
    bool IsVisible() const { return visible; }

    const glm::mat4& GetModel() const { return model; }

protected:
    virtual void Draw();

private:
    void SetRelativePosition(const glm::vec2& position, const Rect& parentRect);
    void CalculateRelativePivotPosition();

public:
    Event<void()> onPositionChanged;
    Event<void()> onSizeChanged;

    bool clipChildren {false};

protected:
    bool visible    {true};
    bool enabled    {true};

    // std::string id; // Used for finding the widget inside a parent
    Widget* parent   {nullptr};

    // Represents a rectangle with x and y set as its the top left corner
    Rect rect;
    glm::vec2 pivot {0.0f, 0.0f};
    Anchor anchor   {Anchor::TopLeft};

    // Position relative to the anchor of the Widget
    glm::vec2 position {glm::vec2{0.0f}};
    // Position of the pivot in Screen space (top-left is 0, 0)
    glm::vec2 absolutePivotPosition {glm::vec2{0.0f}};

    std::vector<Owned<class Widget>> children;

private: 
    glm::mat4 model;
    bool isModelDirty {true};
    bool isOrderDirty {true};
    bool destroy      {false};
    
    // Children are rendered always after their parent, even if the parent have higher renderOrder than its children
    // Smaller is rendered first
    int renderOrder   {0};
    bool hasFocus     {false}; // TODO: Make implementation of this
};

void DebugWidgetWindow();

#endif // __WIDGET_H__
