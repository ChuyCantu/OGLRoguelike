#ifndef __SLIDER_H__
#define __SLIDER_H__

#include "Widget.hpp"
#include "Image.hpp"

enum class Orientation {
    Horizontal, Vertical
};

class Thumb;

class Slider : public Widget {
public:
    Slider();
    Slider(const Rect& rect);
    Slider(const glm::vec2& size);
    ~Slider() override;

    void Draw() override;
    // void HandleInput(EventHandler& eventHandler) override;

    void SetValue(float value);
    float GetValue() const { return value; }

public:
    Event<void(Widget*, float)> onValueChanged;

protected:
    void SetupDefaultValues();
    void UpdateButtonChildrenSize(Widget* source);

    // void OnMouseButtonDown(Widget* source, EventHandler& eventHandler);
    // void OnMouseButtonUp(Widget* source, EventHandler& eventHandler);
    void OnThumbPositionChanged(Widget* source);

public:
    float min {0};
    float max {1};

    float value {0.5};

    Orientation orientation {Orientation::Horizontal};

    Image* background;
    Image* track;
    Thumb* thumb;

    // bool isBeingDragged {false};
};

class Thumb : public Image {
public:
    Thumb(const Rect& rect, Ref<Sprite> sprite);
    ~Thumb() override;

    void HandleInput(EventHandler& eventHandler) override;

protected:
    void OnMouseButtonDown(Widget* source, EventHandler& eventHandler);
    void OnMouseButtonUp(Widget* source, EventHandler& eventHandler);

private:
    glm::vec2 minPosition;
    glm::vec2 maxPosition;
    bool isBeingDragged{false};

    friend class Slider;
};

#endif // __SLIDER_H__