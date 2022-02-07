#include "Button.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "UI.hpp"

Button::Button(const std::string& text) : Widget{} {
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

Button::Button(const std::string& text, const Rect& rect) : Widget{rect} {
    SetupDefaultValues();

    auto lab{AddChild(MakeOwned<Label>(text, 18, rect))};
    label = dynamic_cast<Label*>(lab);
    label->verticalAlign = TextVertAlign::Center;
    label->horizontalAlign = TextHorzAlign::Center;
    label->SetRenderOrder(1);

    childrenIgnoreInput = true;
}

Button::Button(const std::string& text, const glm::vec2& size) : Widget{size} {
    SetupDefaultValues();

    auto lab{AddChild(MakeOwned<Label>(text, 18, rect))};
    label = dynamic_cast<Label*>(lab);
    label->verticalAlign = TextVertAlign::Center;
    label->horizontalAlign = TextHorzAlign::Center;
    label->SetRenderOrder(1);

    childrenIgnoreInput = true;

    onClick.Subscribe("click", 
        [](Widget* source, EventHandler& event) {
            LOG_TRACE("Button clicked");
        });
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

    normal->color = glm::vec4{1.0f};
    pressed->color = glm::vec4{1.0f, 0.f, 1.f, 1.f};
    highlighted->color = glm::vec4{204, 0, 122, 255} / 255.f;
    disabled->color = glm::vec4{51, 50, 50, 255} / 255.f;

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