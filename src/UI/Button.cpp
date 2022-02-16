#include "Button.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "UI.hpp"

Button::Button(const std::string& text, const std::string& name) : Widget{name} {
    auto lab {AddChild(MakeOwned<Label>(text, 18))};
    label = dynamic_cast<Label*>(lab);
    label->SetRenderOrder(1);
    label->verticalAlign = TextVertAlign::Center;
    label->horizontalAlign = TextHorzAlign::Center;
    SetRect(Rect{glm::vec2{0.0f, 0.0f}, label->GetRect().size});

    SetupDefaultValues();

    onSizeChanged.Subscribe("OnSizeChanged", &Button::UpdateButtonChildrenSize, this);

    childrenIgnoreInput = true;
}

Button::Button(const std::string& text, const Rect& rect, const std::string& name) : Widget{rect, name} {
    SetupDefaultValues();

    auto lab{AddChild(MakeOwned<Label>(text, 18, rect))};
    label = dynamic_cast<Label*>(lab);
    label->verticalAlign = TextVertAlign::Center;
    label->horizontalAlign = TextHorzAlign::Center;
    label->SetRenderOrder(1);

    onSizeChanged.Subscribe("OnSizeChanged", &Button::UpdateButtonChildrenSize, this);

    childrenIgnoreInput = true;
}

Button::Button(const std::string& text, const glm::vec2& size, const std::string& name) : Widget{size, name} {
    SetupDefaultValues();

    auto lab{AddChild(MakeOwned<Label>(text, 18, rect))};
    label = dynamic_cast<Label*>(lab);
    label->verticalAlign = TextVertAlign::Center;
    label->horizontalAlign = TextHorzAlign::Center;
    label->SetRenderOrder(1);

    onSizeChanged.Subscribe("OnSizeChanged", &Button::UpdateButtonChildrenSize, this);

    childrenIgnoreInput = true;
}

Button::~Button() {
    onSizeChanged.Unsubscribe("OnSizeChanged", this);
}

void Button::Draw() {
    if (!enabled) {
        normal->SetVisible(false);
        pressed->SetVisible(false);
        highlighted->SetVisible(false);
        disabled->SetVisible(true);
        return;
    }

    if (UI::hovered == this) {
        if (isBeingClicked) {
            normal->SetVisible(false);
            pressed->SetVisible(true);
            highlighted->SetVisible(false);
            disabled->SetVisible(false);
        }
        else {
            normal->SetVisible(false);
            pressed->SetVisible(false);
            highlighted->SetVisible(true);
            disabled->SetVisible(false);
        }
    }
    else {
        normal->SetVisible(true);
        pressed->SetVisible(false);
        highlighted->SetVisible(false);
        disabled->SetVisible(false);
    }
}

void Button::SetupDefaultValues() {
    auto defaultSprite {MakeRef<Sprite>(AssetManager::GetTexture("default"))};

    auto norm {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};
    auto pres {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};
    auto hl   {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};
    auto dis  {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};

    normal = dynamic_cast<Image*>(norm);
    pressed = dynamic_cast<Image*>(pres);
    highlighted = dynamic_cast<Image*>(hl);
    disabled = dynamic_cast<Image*>(dis);

    normal->color = ColorNames::white;
    pressed->color = Color{0.0f, 0.f, 1.f, 1.f};
    highlighted->color = Color{(uint8_t)204, (uint8_t)0, (uint8_t)122, (uint8_t)255};
    disabled->color = Color{(uint8_t)51, (uint8_t)50, (uint8_t)50, (uint8_t)255};

    pressed->SetVisible(false);
    highlighted->SetVisible(false);
    disabled->SetVisible(false);
}

void Button::UpdateButtonChildrenSize(Widget* source) {
    normal->SetSize(rect.size);
    pressed->SetSize(rect.size);
    highlighted->SetSize(rect.size);
    disabled->SetSize(rect.size);
    label->SetSize(rect.size);
}