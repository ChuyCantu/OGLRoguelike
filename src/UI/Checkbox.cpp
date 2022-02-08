#include "Checkbox.hpp"

#include "Core/AssetManager.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"

Checkbox::Checkbox() : Widget{glm::vec2{16.f, 16.f}} {
    childrenIgnoreInput = true;
    SetupDefaultValues();
    onClick.Subscribe("OnClick", &Checkbox::ToggleSChecked, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Checkbox::UpdateCheckboxChildrenSize, this);

}

Checkbox::Checkbox(const Rect& rect) : Widget{rect} {
    childrenIgnoreInput = true;
    SetupDefaultValues();
    onClick.Subscribe("OnClick", &Checkbox::ToggleSChecked, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Checkbox::UpdateCheckboxChildrenSize, this);
}

Checkbox::Checkbox(const glm::vec2& size) : Widget{size} {
    childrenIgnoreInput = true;
    SetupDefaultValues();
    onClick.Subscribe("OnClick", &Checkbox::ToggleSChecked, this);
    onSizeChanged.Subscribe("OnSizeChanged", &Checkbox::UpdateCheckboxChildrenSize, this);

}

Checkbox::~Checkbox() {
    onClick.Unsubscribe("OnClick", this);
    onSizeChanged.Unsubscribe("OnSizeChanged", this);
}

void Checkbox::Draw() {
    if (checked) {
        uncheckedImg->SetVisible(true);
        uncheckedImg->SetVisible(false);
    }
    else {
        uncheckedImg->SetVisible(false);
        uncheckedImg->SetVisible(true);
    }
}

void Checkbox::SetupDefaultValues() {
    auto defaultSprite {MakeRef<Sprite>(AssetManager::GetTexture("default"))};

    auto ch   {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};
    auto unch {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};

    checkedImg = dynamic_cast<Image*>(ch);
    uncheckedImg = dynamic_cast<Image*>(unch);

    checkedImg->color = glm::vec4{0.0f, 0.f, 1.f, 1.f};
    uncheckedImg->color = glm::vec4{1.0f};

    uncheckedImg->SetVisible(false);
}

void Checkbox::UpdateCheckboxChildrenSize(Widget* source) {
    checkedImg->SetSize(rect.size);
    uncheckedImg->SetSize(rect.size);
}

void Checkbox::ToggleSChecked(Widget* source, EventHandler& eventHandler) {
    checked = !checked;
    onValueChanged.Invoke(this, checked);
}