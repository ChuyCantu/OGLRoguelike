#include "ScrollView.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Panel.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "Utils/MathExtras.hpp"

//+ ScrollView =====================================================================
ScrollView::ScrollView() : Widget{Rect{glm::vec2{0.0f, 0.0f}, glm::vec2{100.f, 150.f}}} {
    SetupDefaultValues();
}

ScrollView::ScrollView(const Rect& rect) : Widget{rect} {
    SetupDefaultValues();
}

ScrollView::ScrollView(const glm::vec2& size) : Widget{size} {
    SetupDefaultValues();
}

ScrollView::~ScrollView() {
    onSizeChanged.Unsubscribe("OnSizeChanged", this);
    content->onSizeChanged.Unsubscribe("OnContentSizeChanged", this);
    horizontalScrollbar->onValueChanged.Unsubscribe("OnHorzScrollValueChanged", this);
    verticalScrollbar->onValueChanged.Unsubscribe("OnVertScrollValueChanged", this);
}

void ScrollView::ChangeContentContainer(Owned<Widget> newContent) { 
    // TODO: Test this:
    content->onSizeChanged.Unsubscribe("OnContentSizeChanged", this);

    viewport->RemoveAllChildren();
    content = viewport->AddChild(std::move(newContent));

    content->onSizeChanged.Subscribe("OnContentSizeChanged", &ScrollView::OnContentSizeChanged, this);
}

void ScrollView::SetupDefaultValues() {
    ignoreInput = true;

    auto defaultSprite{MakeRef<Sprite>(AssetManager::GetTexture("default"))};

    auto bg {AddChild(MakeOwned<Image>(Rect{glm::vec2{0.0f, 0.0f}, rect.size}, defaultSprite))};
    auto vp {AddChild(MakeOwned<Panel>(glm::vec2{rect.size.x - 10.f, rect.size.y - 10.f}))};
    auto ct {vp->AddChild(MakeOwned<Panel>(vp->GetSize()))};
    auto hsb{AddChild(MakeOwned<Scrollbar>())};
    auto vsb{AddChild(MakeOwned<Scrollbar>())};

    background = dynamic_cast<Image*>(bg);
    viewport = dynamic_cast<Panel*>(vp);
    content = ct;
    horizontalScrollbar = dynamic_cast<Scrollbar*>(hsb);
    verticalScrollbar = dynamic_cast<Scrollbar*>(vsb);

    background->ignoreInput = true;
    viewport->ignoreInput = true;
    viewport->clipChildren = true;

    background->color = glm::vec4{50, 50, 51, 120} / 255.f;

    horizontalScrollbar->SetOrientation(Orientation::Horizontal);
    horizontalScrollbar->SetSize(glm::vec2{rect.size.x - 10.f, 10.f});
    horizontalScrollbar->SetAnchor(Anchor::BottomLeft);
    horizontalScrollbar->SetPivot(glm::vec2{0.0f, 1.0f});
    horizontalScrollbar->SetPosition(glm::vec2{0.0f});
    horizontalScrollbar->SetThumbSize(1.0f);

    verticalScrollbar->SetSize(glm::vec2{10.f, rect.size.y - 10.f});
    verticalScrollbar->SetAnchor(Anchor::TopRight);
    verticalScrollbar->SetPivot(glm::vec2{1.0f, 0.0f});
    verticalScrollbar->SetPosition(glm::vec2{0.0f});
    verticalScrollbar->SetThumbSize(1.0f);

    onSizeChanged.Subscribe("OnSizeChanged", &ScrollView::UpdateScrollViewChildrenSize, this);
    content->onSizeChanged.Subscribe("OnContentSizeChanged", &ScrollView::OnContentSizeChanged, this);
    horizontalScrollbar->onValueChanged.Subscribe("OnHorzScrollValueChanged", &ScrollView::OnHorzScrollValueChanged, this);
    verticalScrollbar->onValueChanged.Subscribe("OnVertScrollValueChanged", &ScrollView::OnVertScrollValueChanged, this);
}

void ScrollView::UpdateScrollViewChildrenSize(Widget* source) {

}

void ScrollView::OnContentSizeChanged(Widget* source) {
    float hThumbSize {horizontalScrollbar->GetThumbSize()};
    float vThumbSize {verticalScrollbar->GetThumbSize()};

    auto viewport_content_ratio {viewport->GetSize() / content->GetSize()};
    // viewport_content_ratio.x = std::clamp(viewport_content_ratio.x, 0.0f, 1.0f);
    // viewport_content_ratio.y = std::clamp(viewport_content_ratio.y, 0.0f, 1.0f);

    float hScrollSize {horizontalScrollbar->GetSize().x};
    float vScrollSize {verticalScrollbar->GetSize().y};

    float shrinkSpeedx {1.0f};
    float shrinkSpeedy {1.0f};
    if (viewport_content_ratio.x < 0.5f)
        shrinkSpeedx = 0.5f;
    if (viewport_content_ratio.x < 0.5f)
        shrinkSpeedy = 0.5f;

    float hNewSize {std::clamp(hThumbSize * viewport_content_ratio.x * shrinkSpeedx, minThumbSize / hScrollSize, 1.0f)};
    float vNewSize {std::clamp(vThumbSize * viewport_content_ratio.y * shrinkSpeedy, minThumbSize / vScrollSize, 1.0f)};

    horizontalScrollbar->SetThumbSize(hNewSize);
    verticalScrollbar->SetThumbSize(vNewSize);

    if (hNewSize == 1.0f)
        horizontalScrollbar->SetVisible(false);
    else
        horizontalScrollbar->SetVisible(true);

    if (vNewSize == 1.0f)
        verticalScrollbar->SetVisible(false);
    else
        verticalScrollbar->SetVisible(true);

    horizontalScrollbar->SetValue(0.0f);
    verticalScrollbar->SetValue(0.0f);
}

void ScrollView::OnHorzScrollValueChanged(Widget* source, float value) {
    glm::vec2 contentPos   {content->GetPosition()};
    glm::vec2 contentSize  {content->GetSize()};
    glm::vec2 viewportSize {viewport->GetSize()};
    content->SetPosition(glm::vec2{-(contentSize.x - viewportSize.x) * horizontalScrollbar->GetValue(), contentPos.y});
}

void ScrollView::OnVertScrollValueChanged(Widget* source, float value) {
    glm::vec2 contentPos   {content->GetPosition()};
    glm::vec2 contentSize  {content->GetSize()};
    glm::vec2 viewportSize {viewport->GetSize()};
    content->SetPosition(glm::vec2{contentPos.x, -(contentSize.y - viewportSize.y) * verticalScrollbar->GetValue()});
}


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
    this->size = std::clamp(size, 0.0f, 1.0f);
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

void Scrollbar::SetValue(float value) {
    switch (orientation) {
        case Orientation::Horizontal: 
            thumb->SetPosition(glm::vec2{MapValues(value, 0.0f, 1.0f, thumb->minPosition.x, thumb->maxPosition.x), thumb->GetPosition().y});
            break;
        case Orientation::Vertical:
            thumb->SetPosition(glm::vec2{thumb->GetPosition().x, MapValues(value, 1.0f, 0.0f, thumb->minPosition.y, thumb->maxPosition.y)});
            break;
    }

    this->value = value;
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

    onValueChanged.Invoke(this, value);
}

void Scrollbar::OnButtonDown(Widget* source, EventHandler& eventHandler) {
    //! Small hack to make the thumb handle the positioning as usual
    thumb->isBeingDragged = true;
    thumb->HandleInput(eventHandler);
}

void Scrollbar::OnButtonUp(Widget* source, EventHandler& eventHandler) {
    thumb->isBeingDragged = false;
}