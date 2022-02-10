#include "Slider.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Camera.hpp"
#include "Utils/MathExtras.hpp"   

//+ Slider =====================================================================

Slider::Slider() : Widget{glm::vec2{100.f, 10.f}} {
    SetupDefaultValues();

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
    onLeftButtonDown.Subscribe("OnButtonDown", &Slider::OnButtonDown, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Slider::UpdateSliderChildrenSize, this);
    onLeftButtonUp.Subscribe("OnButtonUp", &Slider::OnButtonUp, this);
}

Slider::Slider(const Rect& rect) : Widget{rect} {
    SetupDefaultValues();

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
    onLeftButtonDown.Subscribe("OnButtonDown", &Slider::OnButtonDown, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Slider::UpdateSliderChildrenSize, this);
    onLeftButtonUp.Subscribe("OnButtonUp", &Slider::OnButtonUp, this);
}

Slider::Slider(const glm::vec2& size) : Widget{size} {
    SetupDefaultValues();

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
    onLeftButtonDown.Subscribe("OnButtonDown", &Slider::OnButtonDown, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Slider::UpdateSliderChildrenSize, this);
    onLeftButtonUp.Subscribe("OnButtonUp", &Slider::OnButtonUp, this);
}

Slider::~Slider() {
    thumb->onPositionChanged.Unsubscribe("OnThumbPosChanged", this);
    onLeftButtonDown.Unsubscribe("OnButtonDown", this);
    onLeftButtonUp.Unsubscribe("OnButtonUp", this);
    onSizeChanged.Unsubscribe("OnSizeChanged", this);
}

void Slider::SetValue(float value) {
    if (this->value == value)
        return;

    // this->value = value; // This is not needed since the thumb will update it anyway
    if (wholeNumbers)
        value = std::round(value);

    // Update thumb and track positions
    switch (direction) {
        case SliderDirection::LeftToRight: {
            thumb->SetPosition(glm::vec2{MapValues(value, min, max, thumb->minPosition.x, thumb->maxPosition.x), thumb->GetPosition().y});
            glm::vec2 diff{thumb->GetAbsolutePivotPosition() - track->GetAbsolutePivotPosition()};
            track->SetSize(glm::vec2{std::abs(diff.x), track->GetSize().y});
            break;
        }
        case SliderDirection::RightToLeft: {
            thumb->SetPosition(glm::vec2{MapValues(value, max, min, thumb->minPosition.x, thumb->maxPosition.x), thumb->GetPosition().y});
            glm::vec2 diff{thumb->GetAbsolutePivotPosition() - track->GetAbsolutePivotPosition()};
            track->SetSize(glm::vec2{std::abs(diff.x), track->GetSize().y});
            break;
        }
        case SliderDirection::BottomToTop: {
            thumb->SetPosition(glm::vec2{thumb->GetPosition().x, MapValues(value, min, max, thumb->minPosition.y, thumb->maxPosition.y)});
            glm::vec2 diff{thumb->GetAbsolutePivotPosition() - track->GetAbsolutePivotPosition()};
            track->SetSize(glm::vec2{track->GetSize().x, std::abs(diff.y)});
            break;
        }
        case SliderDirection::TopToBottom: {
            thumb->SetPosition(glm::vec2{thumb->GetPosition().x, MapValues(value, max, min, thumb->minPosition.y, thumb->maxPosition.y)});
            glm::vec2 diff{thumb->GetAbsolutePivotPosition() - track->GetAbsolutePivotPosition()};
            track->SetSize(glm::vec2{track->GetSize().x, std::abs(diff.y)});
            break;
        }
    }
}

void Slider::SetValueWithoutNotify(float value) {
    notifyValueChanged = false;
    SetValue(value);
    notifyValueChanged = true;
}

void Slider::SetRange(float min, float max) {
    value = MapValues(value, this->min, this->max, min, max);

    this->min = min;
    this->max = max;
}

// TODO: Implement different track modes
void Slider::SetTrackMode(TrackMode mode) {
    if (this->trackMode == trackMode)
        return;

    this->trackMode = trackMode;


}

void Slider::SetDirection(SliderDirection direction) {
    if (this->direction == direction)
        return;

    this->direction = direction;

    switch (direction) {
        case SliderDirection::LeftToRight: {
            SetSize(glm::vec2{std::max(rect.size.x, rect.size.y), std::min(rect.size.x, rect.size.y)});
            thumb->movementOrientation = Orientation::Horizontal;
            track->SetAnchor(Anchor::TopLeft);
            track->SetPivot(glm::vec2{0.f, 0.f});
            glm::vec2 sizeDiff{rect.size - track->GetSize()};
            track->SetSize(glm::vec2{std::abs(sizeDiff.x), track->GetSize().y});
            break;
        }
        case SliderDirection::RightToLeft: {
            SetSize(glm::vec2{std::max(rect.size.x, rect.size.y), std::min(rect.size.x, rect.size.y)});
            thumb->movementOrientation = Orientation::Horizontal;
            track->SetAnchor(Anchor::TopRight);
            track->SetPivot(glm::vec2{1.f, 0.f});
            glm::vec2 sizeDiff{rect.size - track->GetSize()};
            track->SetSize(glm::vec2{std::abs(sizeDiff.x), track->GetSize().y});
            break;
        }
        case SliderDirection::BottomToTop: {
            SetSize(glm::vec2{std::min(rect.size.x, rect.size.y), std::max(rect.size.x, rect.size.y)});
            thumb->movementOrientation = Orientation::Vertical;
            track->SetAnchor(Anchor::BottomLeft);
            track->SetPivot(glm::vec2{0.f, 1.f});
            glm::vec2 sizeDiff{rect.size - track->GetSize()};
            track->SetSize(glm::vec2{track->GetSize().x, std::abs(sizeDiff.y)});
            break;
        }
        case SliderDirection::TopToBottom: {
            SetSize(glm::vec2{std::min(rect.size.x, rect.size.y), std::max(rect.size.x, rect.size.y)});
            thumb->movementOrientation = Orientation::Vertical;
            track->SetAnchor(Anchor::TopLeft);
            track->SetPivot(glm::vec2{0.f, 0.f});
            glm::vec2 sizeDiff{rect.size - track->GetSize()};
            track->SetSize(glm::vec2{track->GetSize().x, std::abs(sizeDiff.y)});
            break;
        }
    }
    track->SetPosition(glm::vec2{0.0f});
}

void Slider::UseWholeNumbers(bool value) {
    wholeNumbers = value;

    if (wholeNumbers) {
        float rValue {std::round(this->value)};
        if (rValue != this->value) {
            SetValue(rValue);
        }
    }
}

void Slider::SetupDefaultValues() {
    auto defaultSprite{MakeRef<Sprite>(AssetManager::GetTexture("default"))};

    auto bg {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};
    auto tr {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, glm::vec2{rect.size.x / 2.f, rect.size.y}}, defaultSprite))};
    auto th {AddChild(MakeOwned<Thumb>(Rect{glm::vec2{0.0f, 0.0f}, glm::vec2{16.f}}, defaultSprite))};

    background = dynamic_cast<Image*>(bg);
    track = dynamic_cast<Image*>(tr);
    thumb = dynamic_cast<Thumb*>(th);

    background->color = glm::vec4{50, 50, 51, 255} / 255.f;
    track->color = glm::vec4{0.f, 0.f, 1.0f, 1.f};
    thumb->color = glm::vec4{1.f, 1.f, 1.f, 1.f};

    ignoreInput = false;  // false so it can work as a button
    background->ignoreInput = true;
    track->ignoreInput = true; 
    thumb->ignoreInput = false;

    track->SetRenderOrder(1);
    thumb->SetRenderOrder(2);

    thumb->SetAnchor(Anchor::Center);
    thumb->SetPivot(glm::vec2{0.5f});
    thumb->SetPosition(glm::vec2{/*rect.size.x / 2.f, */0.f});
    thumb->minPosition = glm::vec2{-rect.size.x / 2.f};
    thumb->maxPosition = glm::vec2{rect.size.x / 2.f};
}

void Slider::UpdateSliderChildrenSize(Widget* source) { 
    background->SetSize(rect.size);
    // Set up according to value
    track->SetSize(rect.size);
    // thumb->SetSize(glm::vec2{16.f});
    
    thumb->UpdateRelativePosition();
    thumb->minPosition = glm::vec2{-rect.size / 2.f};
    thumb->maxPosition = glm::vec2{rect.size / 2.f};
}

void Slider::OnThumbPositionChanged(Widget* source) {
    glm::vec2 diff {thumb->GetAbsolutePivotPosition() - track->GetAbsolutePivotPosition()};

    float prevValue  {value};
    float prevRValue {std::round(value)};

    switch (direction) {
        case SliderDirection::LeftToRight: {
            track->SetSize(glm::vec2{std::abs(diff.x), track->GetSize().y});
            value = MapValues(thumb->GetPosition().x, thumb->minPosition.x, thumb->maxPosition.x, min, max);
            break;
        }
        case SliderDirection::RightToLeft: {
            track->SetSize(glm::vec2{std::abs(diff.x), track->GetSize().y});
            value = MapValues(thumb->GetPosition().x, thumb->minPosition.x, thumb->maxPosition.x, max, min);
            break;
        }
        case SliderDirection::BottomToTop: {
            track->SetSize(glm::vec2{track->GetSize().x, std::abs(diff.y)});
            value = MapValues(thumb->GetPosition().y, thumb->minPosition.y, thumb->maxPosition.y, min, max);
            break;
        }
        case SliderDirection::TopToBottom: {
            track->SetSize(glm::vec2{track->GetSize().x, std::abs(diff.y)});
            value = MapValues(thumb->GetPosition().y, thumb->minPosition.y, thumb->maxPosition.y, max, min);
            break;
        }
    }

    if (wholeNumbers) {
        float rValue {std::round(value)};
        if (rValue != value) {
            SetValue(rValue);
        }

        if (prevRValue == value)
            return;
    }

    if (prevValue == value)
        return;

    if (notifyValueChanged)
        onValueChanged.Invoke(this, value);
}

void Slider::OnButtonDown(Widget* source, EventHandler& eventHandler) {
    LOG_TRACE("Clicked track");

    //! Small hack to make the thumb handle the positioning as usual
    thumb->isBeingDragged = true;
    thumb->HandleInput(eventHandler);
}

void Slider::OnButtonUp(Widget* source, EventHandler& eventHandler) {
    thumb->isBeingDragged = false;
}

//+ Thumb =====================================================================

Thumb::Thumb(const Rect& rect, Ref<Sprite> sprite) : Image{rect, sprite} {
    ignoreInput = false;

    onLeftButtonDown.Subscribe("OnMouseButtonDown", &Thumb::OnMouseButtonDown, this);
    onLeftButtonUp.Subscribe("OnMouseButtonUp", &Thumb::OnMouseButtonUp, this);
}

Thumb::~Thumb() {
    onLeftButtonDown.Unsubscribe("OnMouseButtonDown", this);
    onLeftButtonUp.Unsubscribe("OnMouseButtonUp", this);
}

void Thumb::HandleInput(EventHandler& eventHandler) {
    Image::HandleInput(eventHandler);

    if (!isBeingDragged)
        return;

    switch (eventHandler.event->type) {
        case SDL_MOUSEMOTION: {
            glm::vec2 screenScale{Camera::GetMainCamera().GetScreenVsVirtualSizeScaleRatio()};
            glm::vec2 mousePosScaled{glm::vec2{eventHandler.event->motion.x, eventHandler.event->motion.y} * screenScale};
            glm::vec2 diff{mousePosScaled - GetAbsolutePivotPosition()};
            glm::vec2 newPos;
            if (movementOrientation == Orientation::Horizontal)
                newPos = glm::vec2{std::max(std::min(GetPosition().x + diff.x, maxPosition.x), minPosition.x), GetPosition().y};
            else 
                newPos = glm::vec2{GetPosition().x, std::max(std::min(GetPosition().y - diff.y, maxPosition.y), minPosition.y)};
            SetPosition(newPos);
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
            if (eventHandler.event->button.button == SDL_BUTTON_LEFT) {
                glm::vec2 screenScale{Camera::GetMainCamera().GetScreenVsVirtualSizeScaleRatio()};
                glm::vec2 mousePosScaled{glm::vec2{eventHandler.event->button.x, eventHandler.event->button.y} * screenScale};
                glm::vec2 diff{mousePosScaled - GetAbsolutePivotPosition()};
                glm::vec2 newPos;
                if (movementOrientation == Orientation::Horizontal)
                    newPos = glm::vec2{std::max(std::min(GetPosition().x + diff.x, maxPosition.x), minPosition.x), GetPosition().y};
                else
                    newPos = glm::vec2{GetPosition().x, std::max(std::min(GetPosition().y - diff.y, maxPosition.y), minPosition.y)};
                SetPosition(newPos);
            }
            break;
    }
}

void Thumb::OnMouseButtonDown(Widget* source, EventHandler& eventHandler) {
    isBeingDragged = true;
    LOG_TRACE("Begin drag");
}

void Thumb::OnMouseButtonUp(Widget* source, EventHandler& eventHandler) {
    isBeingDragged = false;
    LOG_TRACE("End drag");
}