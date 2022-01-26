#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

struct Rect {
    glm::vec2 position {0.0f};
    glm::vec2 size     {100.0f};
};

enum class Anchor {
    TopLeft,
    TopRight,
    Center,
    BottomLeft,
    BottomRight
};

class Panel;

class Widget {
public:
    Widget(Panel* panel);
    Widget(Panel* panel, const Rect& rect);
    virtual ~Widget();

    // Moves the widget to an absolute position on screen
    void SetPosition(const glm::vec2& position);
    // Moves the widget relative to its parent panel
    void SetRelativePosition(const glm::vec2& position);
    void SetSize(const glm::vec2& size);
    void SetRect(const Rect& rect);
    void SetPivot(const glm::vec2& pivot);
    void SetAnchor(Anchor anchor);

    void UpdateTransform();

    const Rect& GetRect() const { return rect; }
    const glm::vec2& GetPivotPosition() const { return pivotPosition; }
    const glm::vec2& GetPivot() const { return pivot; }
    const Anchor GetAnchor() const { return anchor; }
    // bool IsVisible() const { return isVisible; }
    // bool IsEnabled() const { return enabled; }
    Panel* GetPanel() { return parentPanel; }

    const glm::mat4& GetModel() const { return model; }

public:
    bool isVisible {true};
    bool enabled   {true};

protected:
    // Represents a rectangle with x and y being the top left corner of it
    Rect rect;
    glm::vec2 pivotPosition {glm::vec2{0.0f}};
    glm::vec2 pivot         {0.5f};
    Anchor anchor           {Anchor::TopLeft};

    Panel* parentPanel;

private: 
    glm::mat4 model;
    bool isModelDirty {true};
};

#endif // __WIDGET_H__
