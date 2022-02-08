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
    ignoreInput = true;

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Slider::UpdateSliderChildrenSize, this);
}

Slider::Slider(const Rect& rect) : Widget{rect} {
    SetupDefaultValues();
    ignoreInput = true;

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Slider::UpdateSliderChildrenSize, this);
}

Slider::Slider(const glm::vec2& size) : Widget{size} {
    SetupDefaultValues();
    ignoreInput = true;

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Slider::UpdateSliderChildrenSize, this);
}

Slider::~Slider() {
    thumb->onPositionChanged.Unsubscribe("OnThumbPosChanged", this);
    onSizeChanged.Unsubscribe("OnSizeChanged", this);
}

// TODO: Fix value mapping being inverted in a vertical orientation and value not inverting when the slide is inverted
void Slider::SetValue(float value) {
    this->value = value;
    // Update thumb and track positions
    if (orientation == Orientation::Horizontal) {
        thumb->SetPosition(glm::vec2{MapValues(value, min, max, thumb->minPosition.x, thumb->maxPosition.x), thumb->GetPosition().y});
        glm::vec2 diff {thumb->GetAbsolutePivotPosition() - track->GetAbsolutePivotPosition()};
        track->SetSize(glm::vec2{std::abs(diff.x), track->GetSize().y});

    }
    else {
        thumb->SetPosition(glm::vec2{thumb->GetPosition().x, MapValues(value, min, max, thumb->minPosition.y, thumb->maxPosition.y)});
        glm::vec2 diff {thumb->GetAbsolutePivotPosition() - track->GetAbsolutePivotPosition()};
        track->SetSize(glm::vec2{track->GetSize().x, std::abs(diff.y)});
    }
}

void Slider::SetOrientation(Orientation orientation) {
    this->orientation = orientation;
    SetSize(glm::vec2{rect.size.y, rect.size.x});
    thumb->movementOrientation = orientation;
}

void Slider::Draw() {

}

void Slider::InvertDirection() {
    if (orientation == Orientation::Horizontal) {
        if (track->GetAnchor() == Anchor::TopLeft)  {
            track->SetAnchor(Anchor::TopRight);
            track->SetPivot(glm::vec2{1.f, 0.f});
        }
        else {
            track->SetAnchor(Anchor::TopLeft); 
            track->SetPivot(glm::vec2{0.f, 0.f});
        }
        glm::vec2 sizeDiff{rect.size - track->GetSize()};
        track->SetSize(glm::vec2{std::abs(sizeDiff.x), track->GetSize().y});
    }
    else {
        if (track->GetAnchor() == Anchor::TopLeft) {
            track->SetAnchor(Anchor::BottomLeft);
            track->SetPivot(glm::vec2{0.f, 1.f});
        }
        else {
            track->SetAnchor(Anchor::TopLeft); 
            track->SetPivot(glm::vec2{0.f, 0.f});
        }
        glm::vec2 sizeDiff{rect.size - track->GetSize()};
        track->SetSize(glm::vec2{track->GetSize().x, std::abs(sizeDiff.y)});
    }
    
    track->SetPosition(glm::vec2{0.0f});
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
    thumb->color = glm::vec4{1.f, 1.f, 1.f, 0.5f};

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
    thumb->SetSize(glm::vec2{16.f});
    
    thumb->UpdateRelativePosition();
    thumb->minPosition = glm::vec2{-rect.size / 2.f};
    thumb->maxPosition = glm::vec2{rect.size / 2.f};
}

void Slider::OnThumbPositionChanged(Widget* source) {
    glm::vec2 diff {thumb->GetAbsolutePivotPosition() - track->GetAbsolutePivotPosition()};
    if (orientation == Orientation::Horizontal) {
        track->SetSize(glm::vec2{std::abs(diff.x), track->GetSize().y});
        value = MapValues(thumb->GetPosition().x, thumb->minPosition.x, thumb->maxPosition.x, min, max);
    }
    else {
        track->SetSize(glm::vec2{track->GetSize().x, std::abs(diff.y)});
        value = MapValues(thumb->GetPosition().y, thumb->minPosition.y, thumb->maxPosition.y, min, max);
    }
}


//+ Thumb =====================================================================

Thumb::Thumb(const Rect& rect, Ref<Sprite> sprite) : Image{rect, sprite} {
    ignoreInput = false;

    // onSizeChanged.Subscribe("OnSizeChanged", &Thumb::UpdateButtonChildrenSize, this);
    onLeftButtonDown.Subscribe("OnMouseButtonDown", &Thumb::OnMouseButtonDown, this);
    onLeftButtonUp.Subscribe("OnMouseButtonUp", &Thumb::OnMouseButtonUp, this);
}

Thumb::~Thumb() {
    // onSizeChanged.Unsubscribe("OnSizeChanged", this);
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