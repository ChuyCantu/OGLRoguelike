#ifndef __SLIDER_H__
#define __SLIDER_H__

#include "Widget.hpp"
#include "Image.hpp"

enum class TrackMode {
    Shrink, Clip, Push
};

enum class Orientation {
    Horizontal, Vertical
};

enum class SliderDirection {
    LeftToRight, RightToLeft, 
    BottomToTop, TopToBottom
};

class Thumb;

class Slider : public Widget {
public:
    Slider();
    Slider(const Rect& rect);
    Slider(const glm::vec2& size);
    ~Slider() override;

    void Draw() override { }

    void SetDirection(SliderDirection direction);
    void SetValue(float value);
    void SetTrackMode(TrackMode mode);

    float GetValue() const { return value; }
    SliderDirection GetDirection() const { return direction; }
    TrackMode GetTrackMode() const { return trackMode; }

public:
    Event<void(Widget*, float)> onValueChanged;

protected:
    void SetupDefaultValues();
    void UpdateSliderChildrenSize(Widget* source);

    void OnThumbPositionChanged(Widget* source);
    void OnButtonDown(Widget* source, EventHandler& eventHandler);
    void OnButtonUp(Widget* source, EventHandler& eventHandler);

public:
    float min  {0};
    float max  {1};
    float step {0};

protected:
    float value {0.5};

    SliderDirection direction {SliderDirection::LeftToRight};
    TrackMode trackMode       {TrackMode::Shrink};

    Image* background;
    Image* track;
    Thumb* thumb;
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
    Orientation movementOrientation {Orientation::Horizontal};
    glm::vec2 minPosition;
    glm::vec2 maxPosition;
    bool isBeingDragged{false};

    friend class Slider;
};

#endif // __SLIDER_H__