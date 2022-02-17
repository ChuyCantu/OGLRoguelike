#include "Widget.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Rendering/Batch.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"
#include "UI/UI.hpp"
#include "Utils/Color.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

Widget::Widget(const std::string& name) : name{name} { }

Widget::Widget(const glm::vec2& size, const std::string& name) : name{name}, rect{glm::vec2{0.f}, size} {
    SetAbsolutePosition(rect.position);
}

Widget::Widget(const Rect& rect, const std::string& name) : name{name}, rect{rect} {
    SetAbsolutePosition(rect.position);
}

Widget::~Widget() { }

// TODO: Fix: changing the absolute y position affects the relative x position
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
}

void Widget::RemoveAllChildren() {
    children.clear();
}

Widget* Widget::FindChild(const std::string& name, bool searchInChildren) {
    for (auto& child : children) {
        if (child->name == name)
            return child.get();
    }

    if (searchInChildren) {
        for (auto& child : children) {
            Widget* result {child->FindChild(name, true)};
            if (result != nullptr)
                return result;
        }
    }

    return nullptr;
}

std::vector<Widget*> Widget::FindChildren(const std::string& name) {
    std::vector<Widget*> result;

    for (auto& child : children) {
        if (child->name == name)
            result.emplace_back(child.get());
    }

    return result;
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
        case Anchor::Top: {
            absolutePivotPosition.x = parentSize.x / 2.0f + position.x + parentPos.x;
            absolutePivotPosition.y = position.y + parentPos.y;
            rect.position = absolutePivotPosition - offset;
            break;
        }
        case Anchor::TopRight: {
            absolutePivotPosition.x = parentSize.x - position.x + parentPos.x;
            absolutePivotPosition.y = position.y + parentPos.y;
            rect.position = absolutePivotPosition - offset;
            break;
        }
        case Anchor::Left: {
            absolutePivotPosition.x = position.x + parentPos.x;
            absolutePivotPosition.y = parentSize.y / 2.0f - position.y + parentPos.y;
            rect.position = absolutePivotPosition - offset;
            break;
        }
        case Anchor::Center: {
            absolutePivotPosition.x = parentSize.x / 2.0f + position.x + parentPos.x;
            absolutePivotPosition.y = parentSize.y / 2.0f - position.y + parentPos.y;
            rect.position = absolutePivotPosition - offset;
            break;
        }
        case Anchor::Right: {
            absolutePivotPosition.x = parentSize.x - position.x + parentPos.x;
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
        case Anchor::Bottom: {
            absolutePivotPosition.x = parentSize.x / 2.0f + position.x + parentPos.x;
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
        case Anchor::Top: {
            position.x = -(parentSize.x / 2.0f - absolutePivotPosition.x + parentPos.x);
            position.y = absolutePivotPosition.y - parentPos.y;
            break;
        }
        case Anchor::TopRight: {
            position.x = parentSize.x - absolutePivotPosition.x + parentPos.x;
            position.y = absolutePivotPosition.y - parentPos.y;
            break;
        }
        case Anchor::Left: {
            position.x = absolutePivotPosition.x - parentPos.x;
            position.y = parentSize.y / 2.0f - absolutePivotPosition.y + parentPos.y;
            break;
        }
        case Anchor::Center: {
            position.x = -(parentSize.x / 2.0f - absolutePivotPosition.x + parentPos.x);
            position.y = parentSize.y / 2.0f - absolutePivotPosition.y + parentPos.y;
            break;
        }
        case Anchor::Right: {
            position.x = parentSize.x - absolutePivotPosition.x + parentPos.x;
            position.y = parentSize.y / 2.0f - absolutePivotPosition.y + parentPos.y;
            break;
        }
        case Anchor::BottomLeft: {
            position.x = absolutePivotPosition.x - parentPos.x;
            position.y = parentSize.y - absolutePivotPosition.y + parentPos.y;
            break;
        }
        case Anchor::Bottom: {
            position.x = -(parentSize.x / 2.0f - absolutePivotPosition.x + parentPos.x);
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
#ifdef GUI_BATCH
        //+ Since we're using scissor test, we need to flush the current batch that doesn't use it
        if (SpriteBatch::IsReadyForRendering()) {
            SpriteBatch::Flush(AssetManager::GetShader("gui").get());
        }
#endif

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
#ifdef GUI_BATCH
        //+ Make sure we render the clipped widgets before disabling scissor test
        if (SpriteBatch::IsReadyForRendering()) {
            SpriteBatch::Flush(AssetManager::GetShader("gui").get());
        }
#endif
        glDisable(GL_SCISSOR_TEST);
    }

    RemovePendingChildren();
}

void Widget::Draw() {
#ifdef GUI_BATCH
    if (!SpriteBatch::IsReadyForRendering()) 
        SpriteBatch::Start();
    auto guiShader {AssetManager::GetShader("gui").get()};
    auto missingTex{AssetManager::GetTexture("missing")};
    Sprite tempSprite{missingTex};
    UpdateTransform();
    SpriteBatch::DrawGUISprite(rect, glm::vec3{1.0f, 1.0f, 1.0f}, &tempSprite, ColorNames::white, model, guiShader);
#else
    auto uiShader{AssetManager::GetShader("guiOld")};
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
#endif  // GUI_BATCH
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

void DebugWidget(Widget* widget) {
    auto size {widget->GetSize()};
    auto position{widget->GetPosition()};
    auto absposition {widget->GetAbsolutePivotPosition()};
    auto pivot {widget->GetPivot()};
    auto anchor {static_cast<int>(widget->GetAnchor())};
    auto renderOrder {widget->GetRenderOrder()};

    if (ImGui::InputFloat2("Position", glm::value_ptr(position))) {
        widget->SetPosition(position);
    }
    if (ImGui::InputFloat2("Abs Position", glm::value_ptr(absposition))) {
        widget->SetAbsolutePosition(absposition);
    }
    if (ImGui::InputFloat2("Pivot", glm::value_ptr(pivot))) {
        widget->SetPivot(pivot);
    }
    if (ImGui::InputFloat2("Size", glm::value_ptr(size))) {
        widget->SetSize(size);
    }
    if (ImGui::InputInt("Render Order", &renderOrder)) {
        widget->SetRenderOrder(renderOrder);
    }

    int prevAnchor {anchor};
    ImGui::Text("Anchor");
    ImGui::RadioButton("TopLeft", &anchor, static_cast<int>(Anchor::TopLeft));
    ImGui::RadioButton("Top", &anchor, static_cast<int>(Anchor::Top));
    ImGui::RadioButton("TopRight", &anchor, static_cast<int>(Anchor::TopRight));
    ImGui::RadioButton("Left", &anchor, static_cast<int>(Anchor::Left));
    ImGui::RadioButton("Center", &anchor, static_cast<int>(Anchor::Center));
    ImGui::RadioButton("Right", &anchor, static_cast<int>(Anchor::Right));
    ImGui::RadioButton("BottomLeft", &anchor, static_cast<int>(Anchor::BottomLeft));
    ImGui::RadioButton("Bottom", &anchor, static_cast<int>(Anchor::Bottom));
    ImGui::RadioButton("BottomRight", &anchor, static_cast<int>(Anchor::BottomRight));
    ImGui::End();
    if (anchor != prevAnchor) {
        widget->SetAnchor(static_cast<Anchor>(anchor));
    }
}


void DebugWidgetWindow(Widget* widget, const std::string& windowName) {
    auto size {widget->GetSize()};
    auto position{widget->GetPosition()};
    auto absposition {widget->GetAbsolutePivotPosition()};
    auto pivot {widget->GetPivot()};
    auto anchor {static_cast<int>(widget->GetAnchor())};
    auto renderOrder {widget->GetRenderOrder()};

    ImGui::Begin(windowName.c_str());
    if (ImGui::InputFloat2("Position", glm::value_ptr(position))) {
        widget->SetPosition(position);
    }
    if (ImGui::InputFloat2("Abs Position", glm::value_ptr(absposition))) {
        widget->SetAbsolutePosition(absposition);
    }
    if (ImGui::InputFloat2("Pivot", glm::value_ptr(pivot))) {
        widget->SetPivot(pivot);
    }
    if (ImGui::InputFloat2("Size", glm::value_ptr(size))) {
        widget->SetSize(size);
    }
    if (ImGui::InputInt("Render Order", &renderOrder)) {
        widget->SetRenderOrder(renderOrder);
    }

    int prevAnchor {anchor};
    ImGui::Text("Anchor");
    ImGui::RadioButton("TopLeft", &anchor, static_cast<int>(Anchor::TopLeft));
    ImGui::RadioButton("Top", &anchor, static_cast<int>(Anchor::Top));
    ImGui::RadioButton("TopRight", &anchor, static_cast<int>(Anchor::TopRight));
    ImGui::RadioButton("Left", &anchor, static_cast<int>(Anchor::Left));
    ImGui::RadioButton("Center", &anchor, static_cast<int>(Anchor::Center));
    ImGui::RadioButton("Right", &anchor, static_cast<int>(Anchor::Right));
    ImGui::RadioButton("BottomLeft", &anchor, static_cast<int>(Anchor::BottomLeft));
    ImGui::RadioButton("Bottom", &anchor, static_cast<int>(Anchor::Bottom));
    ImGui::RadioButton("BottomRight", &anchor, static_cast<int>(Anchor::BottomRight));
    ImGui::End();
    if (anchor != prevAnchor) {
        widget->SetAnchor(static_cast<Anchor>(anchor));
    }
    ImGui::End();
}
