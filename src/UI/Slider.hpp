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
    Slider(const std::string& name = "Slider");
    Slider(const Rect& rect, const std::string& name = "Slider");
    Slider(const glm::vec2& size, const std::string& name = "Slider");
    ~Slider() override;

    void Draw() override { }

    void SetDirection(SliderDirection direction);
    void SetValue(float value);
    void SetValueWithoutNotify(float value);
    void SetRange(float min, float max);
    void UseWholeNumbers(bool value);

    float GetValue() const { return value; }
    SliderDirection GetDirection() const { return direction; }
    TrackMode GetTrackMode() const { return trackMode; }
    float GetMin() const { return min; }
    float GetMax() const { return max; }
    bool WholeNumbers() const { return wholeNumbers; }

public:
    Event<void(Widget*, float)> onValueChanged;

protected:
    void SetupDefaultValues();
    void UpdateSliderChildrenSize(Widget* source);

    void OnThumbPositionChanged(Widget* source);
    void OnButtonDown(Widget* source, EventHandler& eventHandler);
    void OnButtonUp(Widget* source, EventHandler& eventHandler);

    //! Temporary here while is not implemented
    void SetTrackMode(TrackMode mode);

protected:
    float min         {0};
    float max         {1};
    bool wholeNumbers {false};

    float value {0.5};

    SliderDirection direction {SliderDirection::LeftToRight};
    TrackMode trackMode       {TrackMode::Shrink};

    Image* background;
    Image* track;
    Thumb* thumb;

private:
    bool notifyValueChanged {true};
};

class Thumb : public Image {
public:
    Thumb(const Rect& rect, Ref<Sprite> sprite, const std::string& name = "Thumb");
    ~Thumb() override;

    void HandleInput(EventHandler& eventHandler) override;

protected:
    void OnMouseButtonDown(Widget* source, EventHandler& eventHandler);
    void OnMouseButtonUp(Widget* source, EventHandler& eventHandler);

public:
    Orientation movementOrientation {Orientation::Horizontal};
    glm::vec2 minPosition;
    glm::vec2 maxPosition;
    bool isBeingDragged{false};
};

#endif // __SLIDER_H__