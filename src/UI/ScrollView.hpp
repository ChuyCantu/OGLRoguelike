#pragma once

#include "Slider.hpp"
#include "Widget.hpp"

class Scrollbar;

class ScrollView : public Widget {
public:
    ScrollView(const std::string& name = "ScrollView");
    ScrollView(const Rect& rect, const std::string& name = "ScrollView");
    ScrollView(const glm::vec2& size, const std::string& name = "ScrollView");
    ~ScrollView() override;

    void Draw() override { }

    void ChangeContentContainer(Owned<Widget> newContent);

    Widget* const Content() { return content; }

public:
    void SetupDefaultValues();
    void UpdateScrollViewChildrenSize(Widget* source);

    void OnContentSizeChanged(Widget* source);
    void OnHorzScrollValueChanged(Widget* source, float value);
    void OnVertScrollValueChanged(Widget* source, float value);

    float minThumbSize {8.f}; // Size in pixels

    Image* background;
    class Panel* viewport;
    Widget* content; 
    Scrollbar* horizontalScrollbar;
    Scrollbar* verticalScrollbar;
};

// This may inherit from Slider in a future
class Scrollbar : public Widget {
public:
    Scrollbar(const std::string& name = "Scrollbar");
    Scrollbar(const Rect& rect, const std::string& name = "Scrollbar");
    Scrollbar(const glm::vec2& size, const std::string& name = "Scrollbar");
    ~Scrollbar() override;

    void Draw() override { }

    void SetOrientation(Orientation orientation);
    void SetThumbSize(float size);
    void SetValue(float value);

    float GetValue() const { return value; }
    float GetThumbSize() const { return size; }

    Event<void(Widget*, float)> onValueChanged;

protected:
    void SetupDefaultValues();
    void UpdateScrollbarChildrenSize(Widget* source);

    void OnThumbPositionChanged(Widget* source);
    void OnButtonDown(Widget* source, EventHandler& eventHandler);
    void OnButtonUp(Widget* source, EventHandler& eventHandler);

public:
    Orientation orientation {Orientation::Vertical};

    float value {1.0f}; // [0.0 - 1.0]
    float size  {0.5f}; // [0.0 - 1.0]

    Image* background;
    Thumb* thumb;
};