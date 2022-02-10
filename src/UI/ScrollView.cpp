#include "ScrollView.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Utils/MathExtras.hpp"

//+ Scrollbar =====================================================================
// TODO: Clean up and refactor scrollbar implementation
Scrollbar::Scrollbar() : Widget{glm::vec2{10.f, 100.f}} {
    SetupDefaultValues();
}

Scrollbar::Scrollbar(const Rect& rect) : Widget{rect} {
    SetupDefaultValues();
}

Scrollbar::Scrollbar(const glm::vec2& size) : Widget{size} {
    SetupDefaultValues();
}

Scrollbar::~Scrollbar() {
    thumb->onPositionChanged.Unsubscribe("OnThumbPosChanged", this);
    onLeftButtonDown.Unsubscribe("OnButtonDown", this);
    onSizeChanged.Unsubscribe("OnSizeChanged", this);
    onLeftButtonUp.Unsubscribe("OnButtonUp", this);
}

void Scrollbar::SetOrientation(Orientation orientation) {
    if (this->orientation == orientation)
        return;

    this->orientation = orientation;

    switch (orientation) {
        case Orientation::Horizontal:
            thumb->movementOrientation = Orientation::Horizontal;
            SetSize(glm::vec2{std::max(rect.size.x, rect.size.y), std::min(rect.size.x, rect.size.y)});
            break;
        case Orientation::Vertical:
            thumb->movementOrientation = Orientation::Vertical;
            SetSize(glm::vec2{std::min(rect.size.x, rect.size.y), std::max(rect.size.x, rect.size.y)});
            break;
    }
}

void Scrollbar::SetThumbSize(float size) {
    this->size = size;
    float tempValue {value};

    switch (orientation) {
        case Orientation::Horizontal: {
            thumb->minPosition = glm::vec2{(-rect.size.x + rect.size.x * size) / 2.0f};
            thumb->maxPosition = glm::vec2{(rect.size.x - rect.size.x * size) / 2.0f};
            thumb->SetSize(glm::vec2{rect.size.x * size, rect.size.y});
            thumb->SetPosition(glm::vec2{MapValues(tempValue, 0.0f, 1.0f, thumb->minPosition.x, thumb->maxPosition.x), 0.0f});
            break;
        }
        case Orientation::Vertical: {
            thumb->minPosition = glm::vec2{(-rect.size.y + rect.size.y * size) / 2.0f};
            thumb->maxPosition = glm::vec2{(rect.size.y - rect.size.y * size) / 2.0f};
            thumb->SetSize(glm::vec2{rect.size.x, rect.size.y * size});
            thumb->SetPosition(glm::vec2{0.0f, MapValues(tempValue, 1.0f, 0.0f, thumb->minPosition.y, thumb->maxPosition.y)});
            break;
        }
    }

    value = tempValue;
}

void Scrollbar::SetupDefaultValues() {
    auto defaultSprite{MakeRef<Sprite>(AssetManager::GetTexture("default"))};

    auto bg{AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};
    auto th{AddChild(MakeOwned<Thumb>(Rect{glm::vec2{0.0f, 0.0f}, glm::vec2{rect.size.x, rect.size.y * size}}, defaultSprite))};

    background = dynamic_cast<Image*>(bg);
    thumb = dynamic_cast<Thumb*>(th);

    background->color = glm::vec4{50, 50, 51, 255} / 255.f;
    thumb->color = glm::vec4{1.f, 1.f, 1.f, 1.f};

    ignoreInput = false;  // false so it can work as a button
    background->ignoreInput = true;
    thumb->ignoreInput = false;

    thumb->SetRenderOrder(1);

    thumb->SetAnchor(Anchor::Center);
    thumb->SetPivot(glm::vec2{0.5f});
    thumb->minPosition = glm::vec2{(-rect.size.y + thumb->GetSize().y) / 2.0f};
    thumb->maxPosition = glm::vec2{(rect.size.y - thumb->GetSize().y) / 2.0f};
    thumb->SetPosition(glm::vec2{0.f, thumb->maxPosition.y});
    thumb->movementOrientation = Orientation::Vertical;

    thumb->onPositionChanged.Subscribe("OnThumbPosChanged", &Scrollbar::OnThumbPositionChanged, this);
    onLeftButtonDown.Subscribe("OnButtonDown", &Scrollbar::OnButtonDown, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Scrollbar::UpdateScrollbarChildrenSize, this);
    onLeftButtonUp.Subscribe("OnButtonUp", &Scrollbar::OnButtonUp, this);
}

void Scrollbar::UpdateScrollbarChildrenSize(Widget* source) {
    background->SetSize(rect.size);

    switch (orientation) {
        case Orientation::Horizontal:
            thumb->SetSize(glm::vec2{rect.size.x * size, rect.size.y});
            break;
        case Orientation::Vertical:
            thumb->SetSize(glm::vec2{rect.size.x, rect.size.y * size});
            break;
    }
    thumb->minPosition = (-rect.size + thumb->GetSize()) / 2.0f;
    thumb->maxPosition = (rect.size - thumb->GetSize()) / 2.0f;
    switch (orientation) {
        case Orientation::Horizontal:
            thumb->SetPosition(glm::vec2{MapValues(value, 0.0f, 1.0f, thumb->minPosition.x, thumb->maxPosition.x), 0.0f});
            break;
        case Orientation::Vertical:
            thumb->SetPosition(glm::vec2{0.0f, MapValues(value, 1.0f, 0.0f, thumb->minPosition.y, thumb->maxPosition.y)});
            break;
    }
}

void Scrollbar::OnThumbPositionChanged(Widget* source) {
    switch (orientation) {
        case Orientation::Horizontal: {
            value = MapValues(thumb->GetPosition().x, thumb->minPosition.x, thumb->maxPosition.x, 0.0f, 1.0f);
            break;
        }
        case Orientation::Vertical: {
            value = MapValues(thumb->GetPosition().y, thumb->minPosition.y, thumb->maxPosition.y, 1.0f, 0.0f);
            break;
        }
    }
}

void Scrollbar::OnButtonDown(Widget* source, EventHandler& eventHandler) {
    //! Small hack to make the thumb handle the positioning as usual
    thumb->isBeingDragged = true;
    thumb->HandleInput(eventHandler);
}

void Scrollbar::OnButtonUp(Widget* source, EventHandler& eventHandler) {
    thumb->isBeingDragged = false;
}