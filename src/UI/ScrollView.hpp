#ifndef __SCROLLVIEW_H__
#define __SCROLLVIEW_H__

#include "Slider.hpp"
#include "Widget.hpp"

// class ScrollView : public Widget {
// public:
//     ScrollView();
//     ScrollView(const Rect& rect);
//     ScrollView(const glm::vec2& size);
//     ~ScrollView() override;

//     Image* background;
//     Widget* viewport;
//     Widget* content; 
//     Scrollbar* horizontalScrollbar;
//     Scrollbar* verticalScrollbar;
// };

// This may inherit from Slider in a future
class Scrollbar : public Widget {
public:
    Scrollbar();
    Scrollbar(const Rect& rect);
    Scrollbar(const glm::vec2& size);
    ~Scrollbar() override;

    void Draw() override { }

    void SetOrientation(Orientation orientation);
    void SetThumbSize(float size);

    float GetValue() const { return value; }
    float GetThumbSize() const { return size; }

public:
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

#endif // __SCROLLVIEW_H__