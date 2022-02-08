#include "Slider.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Camera.hpp"

//+ Slider =====================================================================

Slider::Slider() : Widget{glm::vec2{100.f, 10.f}} {
    SetupDefaultValues();
    ignoreInput = true;

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
}

Slider::Slider(const Rect& rect) : Widget{rect} {
    SetupDefaultValues();
    ignoreInput = true;

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
}

Slider::Slider(const glm::vec2& size) : Widget{size} {
    SetupDefaultValues();
    ignoreInput = true;

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Slider::OnThumbPositionChanged, this);
}

Slider::~Slider() {
    thumb->onPositionChanged.Unsubscribe("OnThumbPosChanged", this);
}

void Slider::SetValue(float value) {
    this->value = value;
    // Update thumb and track positions

}

void Slider::Draw() {

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

void Slider::UpdateButtonChildrenSize(Widget* source) { 
    background->SetSize(rect.size);
    // Set up according to value
    track->SetSize(rect.size);
    thumb->SetSize(glm::vec2{16.f});
    
    thumb->SetPivot(glm::vec2{0.5f});
    thumb->SetPosition(glm::vec2{rect.size.x / 2.f, 0.f});
}

void Slider::OnThumbPositionChanged(Widget* source) {
    glm::vec2 diff {thumb->GetRect().position.x - track->GetRect().position.x};
    track->SetSize(glm::vec2{diff.x, track->GetSize().y});
}

// void Slider::OnMouseButtonDown(Widget* source, EventHandler& eventHandler) {
//     thumb->isBeingDragged = true;
//     LOG_TRACE("Begin drag");
// }

// void Slider::OnMouseButtonUp(Widget* source, EventHandler& eventHandler) {
//     thumb->isBeingDragged = false;
//     LOG_TRACE("End drag");
// }

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
            glm::vec2 newPos{std::max(std::min(GetPosition().x + diff.x, maxPosition.x), minPosition.y), GetPosition().y};
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