#pragma once

#include "Common.hpp"
#include "Core/Event.hpp"
#include "Rect.hpp"

#include <SDL.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#define GUI_BATCH

enum class Anchor {
    TopLeft,    Top,    TopRight,
    Left,       Center, Right,
    BottomLeft, Bottom, BottomRight
};

struct EventHandler {
    SDL_Event* event;
    bool handled {false};
};

class Widget {
public:
    Widget(const std::string& name = "Widget");
    Widget(const Rect& rect, const std::string& name = "Widget");
    Widget(const glm::vec2& size, const std::string& name = "Widget");

    Widget(const Widget&) = delete;
    Widget(const Widget&&) = delete;

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

    // These should only be handled by the UIStack
    void Render();
    virtual void HandleInput(EventHandler& eventHandler);

    Widget* AddChild(Owned<Widget> child);
    void RemoveChild(Widget* child);
    void RemoveAllChildren();

    // Find the first direct child with the given name
    Widget* FindChild(const std::string& name, bool searchInChildren = false);
    // Return all direct children with the given name
    std::vector<Widget*> FindChildren(const std::string& name);
    
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
    const std::vector<Owned<class Widget>>& GetChildren() { return children; }

    const glm::mat4& GetModel() const { return model; }

protected:
    virtual void Draw();

    void SortChildren();
    void RemovePendingChildren();

private:
    void SetRelativePosition(const glm::vec2& position, const Rect& parentRect);
    void CalculateRelativePivotPosition();

public:
    /**
     *  Events include a Widget* that is the source of the event 
     *  and may also include an EventHandler& reference with the SDL event
     *  and a bool handled that can be set to false if we want to keep propagating
     *  the input until another element handles it.
     */
    Event<void(Widget*)> onPositionChanged;
    Event<void(Widget*)> onSizeChanged;
    Event<void(Widget*, EventHandler&)> onClick;
    Event<void(Widget*, EventHandler&)> onLeftButtonDown;
    Event<void(Widget*, EventHandler&)> onLeftButtonUp;
    Event<void(Widget*, EventHandler&)> onMouseEnter;
    Event<void(Widget*, EventHandler&)> onMouseExit;

    std::string name;
    bool clipChildren        {false};
    bool ignoreInput         {false};
    bool childrenIgnoreInput {false};

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

    //+ Input utility:
    bool isBeingHovered {false};
    bool isBeingClicked {false};

private: 
    glm::mat4 model;

    // Children are rendered always after their parent, even if the parent have higher renderOrder than its children
    // Smaller is rendered first
    int renderOrder   {0};

    bool isModelDirty         {true};
    bool needReordering       {false};
    bool destroy              {false};
    bool needChildrenDeletion {false};
};

void DebugWidget(Widget* widget);
void DebugWidgetWindow(Widget* widget, const std::string& windowName);