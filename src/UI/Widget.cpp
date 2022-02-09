#include "Widget.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "UI/UI.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

Widget::Widget() { }

Widget::Widget(const glm::vec2& size) : rect{glm::vec2{0.f}, size} {
    SetAbsolutePosition(rect.position);
}

Widget::Widget(const Rect& rect) : rect{rect} {
    SetAbsolutePosition(rect.position);
}

Widget::~Widget() { }

void Widget::SetAbsolutePosition(const glm::vec2& position) {
    SetRelativePosition(position, Rect{glm::vec2{0, 0}, Camera::GetMainCamera().GetVirtualSize()});
    UpdateChildrenPositions();
    onPositionChanged.Invoke(this);
}

void Widget::SetPosition(const glm::vec2& position) {
    if (parent)
        SetRelativePosition(position, parent->GetRect());
    else
        SetRelativePosition(position, Rect{glm::vec2{0, 0}, Camera::GetMainCamera().GetVirtualSize()});
    UpdateChildrenPositions();
    onPositionChanged.Invoke(this);
}

void Widget::SetSize(const glm::vec2& size) {
    isModelDirty = true;
    rect.size = size;
    UpdateRelativePosition();
    UpdateChildrenPositions();
    onSizeChanged.Invoke(this);
}

void Widget::SetRect(const Rect& rect) {
    isModelDirty = true;
    auto diff {rect.position - this->rect.position};
    absolutePivotPosition += diff;

    CalculateRelativePivotPosition();

    this->rect = rect;

    UpdateChildrenPositions();
    onPositionChanged.Invoke(this);
    onSizeChanged.Invoke(this);
}

void Widget::SetPivot(const glm::vec2& pivot) {
    absolutePivotPosition = rect.position + pivot * rect.size;
    position = position + rect.size * (pivot - this->pivot);
    this->pivot = pivot;
}

void Widget::SetAnchor(Anchor anchor) {
    if (this->anchor == anchor)
        return;

    this->anchor = anchor;

    CalculateRelativePivotPosition();
}

void Widget::SetRenderOrder(int value) {
    renderOrder = value;
    if (parent)
        parent->needReordering = true;
}

void Widget::SetEnabled(bool enabled) {
    if (enabled == this->enabled)
        return;

    this->enabled = enabled;
    for (auto& child : children) {
        child->SetEnabled(enabled);
    }
}

void Widget::SetVisible(bool visible) {
    if (visible == this->visible)
        return;

    this->visible = visible;
    for (auto& child : children) {
        child->SetVisible(visible);
    }
}


Widget* Widget::AddChild(Owned<Widget> child) {
    needReordering = true;
    child->parent = this;
    child->UpdateRelativePosition();
    return children.emplace_back(std::move(child)).get();
}

void Widget::RemoveChild(Widget* child) {
    child->destroy = true;
    needChildrenDeletion = true;

    // auto iter {std::find_if(children.begin(), children.end(),
    //     [&child](const Owned<Widget>& w) {
    //         return w.get() == child;
    //     }
    // )};
    // if (iter != children.end())
    //     children.erase(iter);
}

void Widget::RemoveAllChildren() {
    children.clear();
}

void Widget::UpdateChildrenPositions() {
    for (auto& child: children) {
        child->UpdateRelativePosition();
    }
}

void Widget::UpdateRelativePosition() {
    SetPosition(position);
    UpdateChildrenPositions();
}

void Widget::UpdateTransform() {
    if (!isModelDirty || !visible) 
        return;

    model = glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x, rect.position.y, 0.0f});
    model = glm::scale(model, glm::vec3{rect.size.x, rect.size.y, 1.0f});

    isModelDirty = false;
}

// TODO: Add support for Left, Top, Right and Bottom anchors
void Widget::SetRelativePosition(const glm::vec2& position, const Rect& parentRect) {
    isModelDirty = true;
    this->position = position;

    glm::vec2 offset = pivot * rect.size;
    auto& parentPos{parentRect.position};
    auto& parentSize{parentRect.size};

    switch (anchor) {
        case Anchor::TopLeft: {
            absolutePivotPosition = position + parentPos;
            rect.position = absolutePivotPosition - offset;
            break;
        }
        case Anchor::TopRight: {
            absolutePivotPosition.x = parentSize.x - position.x + parentPos.x;
            absolutePivotPosition.y = position.y + parentPos.y;
            rect.position = absolutePivotPosition - offset;
            break;
        }
        case Anchor::Center: {
            absolutePivotPosition.x = parentSize.x / 2.0f + position.x + parentPos.x;
            absolutePivotPosition.y = parentSize.y / 2.0f - position.y + parentPos.y;
            rect.position = absolutePivotPosition - offset;
            break;
        }
        case Anchor::BottomLeft: {
            absolutePivotPosition.x = position.x + parentPos.x;
            absolutePivotPosition.y = parentSize.y - position.y + parentPos.y;
            rect.position = absolutePivotPosition - offset;
            break;
        }
        case Anchor::BottomRight: {
            absolutePivotPosition.x = parentSize.x - position.x + parentPos.x;
            absolutePivotPosition.y = parentSize.y - position.y + parentPos.y;
            rect.position = absolutePivotPosition - offset;
            break;
        }
    }
}

void Widget::CalculateRelativePivotPosition() {
    glm::vec2 parentPos;
    glm::vec2 parentSize;

    if (parent) {
        parentPos = parent->GetRect().position;
        parentSize = parent->GetRect().size;
    }
    else {
        parentPos = glm::vec2{0.0f, 0.0f};
        parentSize = Camera::GetMainCamera().GetVirtualSize();
    }

    switch (anchor) {
        case Anchor::TopLeft: {
            position = absolutePivotPosition - parentPos;
            break;
        }
        case Anchor::TopRight: {
            position.x = parentSize.x - absolutePivotPosition.x + parentPos.x;
            position.y = absolutePivotPosition.y - parentPos.y;
            break;
        }
        case Anchor::Center: {
            position.x = -(parentSize.x / 2.0f - absolutePivotPosition.x + parentPos.x);
            position.y = parentSize.y / 2.0f - absolutePivotPosition.y + parentPos.y;
            break;
        }
        case Anchor::BottomLeft: {
            position.x = absolutePivotPosition.x - parentPos.x;
            position.y = parentSize.y - absolutePivotPosition.y + parentPos.y;
            break;
        }
        case Anchor::BottomRight: {
            position.x = parentSize.x - absolutePivotPosition.x + parentPos.x;
            position.y = parentSize.y - absolutePivotPosition.y + parentPos.y;
            break;
        }
    }
}

void Widget::SortChildren() {
    if (needReordering) { //! First frame after this is set to true may not be ordered
        std::sort(children.begin(), children.end(), 
            [](const Owned<Widget>& a, const Owned<Widget>& b) {
                return a->GetRenderOrder() < b->GetRenderOrder();
            }
        );
        needReordering = false;
    }
}

void Widget::RemovePendingChildren() {
    if (needChildrenDeletion) {
        needChildrenDeletion = false;

        children.erase(std::remove_if(children.begin(), children.end(),
            [](Owned<Widget>& widget) {
                return widget->destroy;
            }
        ), children.end());
    };
}

void Widget::Render() {
    SortChildren();

    Draw();

    if (clipChildren) {
        glEnable(GL_SCISSOR_TEST);
        Rect scaledRect {Camera::GetMainCamera().RectFromVirtual2ScreenSize(rect, true)};
        glScissor(static_cast<int>(scaledRect.position.x), 
                  static_cast<int>(scaledRect.position.y), 
                  static_cast<int>(scaledRect.size.x), 
                  static_cast<int>(scaledRect.size.y));
    }

    for (auto& child : children) {
        if (child->visible)
            child->Render();
    }

    if (clipChildren) {
        glDisable(GL_SCISSOR_TEST);
    }

    RemovePendingChildren();
}

void Widget::Draw() {
    auto uiShader{AssetManager::GetShader("gui")};
    uiShader->Use();
    auto missingTex{AssetManager::GetTexture("missing")};
    Sprite tempSprite{missingTex};
    missingTex->Use();

    UpdateTransform();
    uiShader->SetMatrix4("model", model);
    uiShader->SetVec2("spriteMinUV", tempSprite.GetMinUV());
    uiShader->SetVec2("spriteMaxUV", tempSprite.GetMaxUV());
    uiShader->SetVec4("color", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    uiShader->SetBool("flipX", false);
    uiShader->SetBool("flipY", false);
    uiShader->SetBool("useVirtualResolution", true);
    AssetManager::GetVertexArray("gui")->Draw();
}

// TODO: Add handle for OnButtonDown and OnButtonUp
void Widget::HandleInput(EventHandler& eventHandler) {
    switch (eventHandler.event->type) {
        // case SDL_KEYDOWN:
        //     if (isMouseInside)
        //         LOG_TRACE("keydown: {}. order: {}. parent: {}.", eventHandler.event->key.keysym.sym, renderOrder, parent->renderOrder);
        //     break;
        case SDL_MOUSEBUTTONDOWN:
            switch (eventHandler.event->button.button) {
                case SDL_BUTTON_LEFT: {
                    // glm::vec2 screenScale{Camera::GetMainCamera().GetScreenVsVirtualSizeScaleRatio()};
                    // glm::ivec2 mousePosScaled{glm::vec2{eventHandler.event->motion.x, eventHandler.event->motion.y} * screenScale};
                    // bool isMouseInside = rect.IsPointInRect(mousePosScaled);
                    // if (isMouseInside) {
                    if (isBeingHovered) {
                        UI::focused = this;
                        eventHandler.handled = true;
                        // onClick.Invoke(this, eventHandler);
                        onLeftButtonDown.Invoke(this, eventHandler);
                        isBeingClicked = true;
                    }
                    else {
                        if (UI::focused == this)
                            UI::focused = nullptr;
                    }
                    break;
                // case SDL_BUTTON_RIGHT:
                //     if (isMouseInside) {
                //         eventHandler.handled = true;
                //         LOG_TRACE("mbuttondown: {}. order: {}. parent: {}.", eventHandler.event->button.button, renderOrder, parent->renderOrder);
                //     }
                //     break;
                }
                default:
                    break;
            }
            break;
        case SDL_MOUSEBUTTONUP: 
            switch (eventHandler.event->button.button) {
                case SDL_BUTTON_LEFT: {
                    if (isBeingClicked)
                        onLeftButtonUp.Invoke(this, eventHandler);
                    isBeingClicked = false;
                    if (isBeingHovered)
                        onClick.Invoke(this, eventHandler);
                    break;
                }
                default:
                    break;
            }
            break;
        case SDL_MOUSEMOTION: {
            glm::vec2 screenScale {Camera::GetMainCamera().GetScreenVsVirtualSizeScaleRatio()};
            glm::ivec2 mousePosScaled{glm::vec2{eventHandler.event->motion.x, eventHandler.event->motion.y} * screenScale};
            bool isMouseInside = rect.IsPointInRect(mousePosScaled);
            if (isMouseInside) {
                eventHandler.handled = true;

                if (!isBeingHovered) {
                    onMouseEnter.Invoke(this, eventHandler);
                    if (UI::hovered != this) 
                        UI::hovered = this;
                }
                isBeingHovered = true;
            }
            else {
                if (isBeingHovered) {
                    isBeingHovered = false;
                    onMouseExit.Invoke(this, eventHandler);
                    if (UI::hovered == this) {
                        UI::hovered = nullptr;
                    }
                }
            }
            break;
        }
    }
}

void DebugWidgetWindow() {
    
}
