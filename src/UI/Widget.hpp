#ifndef __WIDGET_H__
#define __WIDGET_H__

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

    // Sets the position and size of the widget as if the anchor was set to top left
    // without modifing the pivot and anchor values of the widget
    const Rect& GetRect() const { return rect; }
    const Anchor GetAnchor() const { return anchor; }
    const glm::vec2& GetPivot() const { return pivot; }
    const glm::vec2& GetPivotPosition() const { return pivotPosition; }
    const glm::vec2& GetRelativePivotPosition() const { return relativePivotPosition; }
    Panel* GetPanel() { return parentPanel; }

    const glm::mat4& GetModel() const { return model; }

private:
    void AdjustPositionToAnchor(const glm::vec2& position);

public:
    bool visible {true};
    bool enabled   {true};

protected:
    // Represents a rectangle with x and y being the top left corner of it
    Rect rect;
    glm::vec2 pivot                 {0.5f};
    Anchor anchor                   {Anchor::TopLeft};

    // Position relative to the anchor of the Widget
    glm::vec2 relativePivotPosition {glm::vec2{0.0f}};
    // Position of the pivot in Screen space (top-left is 0, 0)
    glm::vec2 pivotPosition         {glm::vec2{0.0f}};

    Panel* parentPanel;

private: 
    glm::mat4 model;
    bool isModelDirty {true};
};

#endif // __WIDGET_H__
