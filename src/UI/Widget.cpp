#include "Widget.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Panel.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Sprite.hpp"
#include "Rendering/Texture.hpp"

#include <glm/gtc/matrix_transform.hpp>

Widget::Widget() { }

Widget::Widget(const Rect& rect) : rect{rect} { }

Widget::~Widget() { }

void Widget::SetPosition(const glm::vec2& position) {
    isModelDirty = true;
    relativePivotPosition = position;

    // AdjustPositionToAnchor(position);

    glm::vec2 offset = pivot * rect.size;
    auto& parentPos{parentPanel->GetRect().position};
    auto& parentSize{parentPanel->GetRect().size};

    switch (anchor) {
        case Anchor::TopLeft: {
            pivotPosition = position;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::TopRight: {
            pivotPosition.x = parentSize.x - position.x;
            pivotPosition.y = position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::Center: {
            pivotPosition.x = parentSize.x / 2.0f + position.x;
            pivotPosition.y = parentSize.y / 2.0f - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::BottomLeft: {
            pivotPosition.x = position.x;
            pivotPosition.y = parentSize.y - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::BottomRight: {
            pivotPosition.x = parentSize.x - position.x;
            pivotPosition.y = parentSize.y - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
    }
}

void Widget::SetRelativePosition(const glm::vec2& position) {
    isModelDirty = true;
    relativePivotPosition = position;

    glm::vec2 offset = pivot * rect.size;
    auto& parentPos{parentPanel->GetRect().position};
    auto& parentSize{parentPanel->GetRect().size};

    switch (anchor) {
        case Anchor::TopLeft: {
            pivotPosition = position + parentPos;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::TopRight: {
            pivotPosition.x = parentSize.x - position.x + parentPos.x;
            pivotPosition.y = position.y + parentPos.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::Center: {
            pivotPosition.x = parentSize.x / 2.0f + position.x + parentPos.x;
            pivotPosition.y = parentSize.y / 2.0f - position.y + parentPos.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::BottomLeft: {
            pivotPosition.x = position.x + parentPos.x;
            pivotPosition.y = parentSize.y - position.y + parentPos.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::BottomRight: {
            pivotPosition.x = parentSize.x - position.x + parentPos.x;
            pivotPosition.y = parentSize.y - position.y + parentPos.y;
            rect.position = pivotPosition - offset;
            break;
        }
    }
}

void Widget::SetSize(const glm::vec2& size) {
    isModelDirty = true;
    rect.size = size;
}

void Widget::SetRect(const Rect& rect) {
    isModelDirty = true;
    auto diff {rect.position - this->rect.position};
    pivotPosition += diff;

    CalculateRelativePivotPosition();

    // auto& parentPos{parentPanel->GetRect().position};
    // auto& parentSize{parentPanel->GetRect().size};

    // switch (anchor) {
    //     case Anchor::TopLeft: {
    //         relativePivotPosition = pivotPosition - parentPos;
    //         break;
    //     }
    //     case Anchor::TopRight: {
    //         relativePivotPosition.x = parentSize.x - pivotPosition.x + parentPos.x;
    //         relativePivotPosition.y = pivotPosition.y - parentPos.y;
    //         break;
    //     }
    //     case Anchor::Center: {
    //         relativePivotPosition.x = -(parentSize.x / 2.0f - pivotPosition.x + parentPos.x);
    //         relativePivotPosition.y = parentSize.y / 2.0f - pivotPosition.y + parentPos.y;
    //         break;
    //     }
    //     case Anchor::BottomLeft: {
    //         relativePivotPosition.x = pivotPosition.x - parentPos.x;
    //         relativePivotPosition.y = parentSize.y - pivotPosition.y + parentPos.y;
    //         break;
    //     }
    //     case Anchor::BottomRight: {
    //         relativePivotPosition.x = parentSize.x - pivotPosition.x + parentPos.x;
    //         relativePivotPosition.y = parentSize.y - pivotPosition.y + parentPos.y;
    //         break;
    //     }
    // }

    this->rect = rect;
}

void Widget::SetPivot(const glm::vec2& pivot) {
    pivotPosition = rect.position + pivot * rect.size;
    relativePivotPosition = relativePivotPosition + rect.size * (pivot - this->pivot);
    this->pivot = pivot;
}

void Widget::SetAnchor(Anchor anchor) {
    this->anchor = anchor;

    // glm::vec2 offset = pivot * rect.size;

    // switch (anchor) {
    //     case Anchor::TopLeft: {
    //         relativePivotPosition = pivotPosition;
    //         break;
    //     }
    //     case Anchor::TopRight: {
    //         relativePivotPosition.x = parentPanel->GetRect().size.x - pivotPosition.x;
    //         relativePivotPosition.y = pivotPosition.y;
    //         break;
    //     }
    //     case Anchor::Center: {
    //         relativePivotPosition.x = -(parentPanel->GetRect().size.x / 2.0f - pivotPosition.x);
    //         relativePivotPosition.y = parentPanel->GetRect().size.y / 2.0f - pivotPosition.y;
    //         break;
    //     }
    //     case Anchor::BottomLeft: {
    //         relativePivotPosition.x = pivotPosition.x;
    //         relativePivotPosition.y = parentPanel->GetRect().size.y - pivotPosition.y;
    //         break;
    //     }
    //     case Anchor::BottomRight: {
    //         relativePivotPosition.x = parentPanel->GetRect().size.x - pivotPosition.x;
    //         relativePivotPosition.y = parentPanel->GetRect().size.y - pivotPosition.y;
    //         break;
    //     }
    // }

    CalculateRelativePivotPosition();

    // auto& parentPos{parentPanel->GetRect().position};
    // auto& parentSize{parentPanel->GetRect().size};

    // switch (anchor) {
    //     case Anchor::TopLeft: {
    //         relativePivotPosition = pivotPosition - parentPos;
    //         break;
    //     }
    //     case Anchor::TopRight: {
    //         relativePivotPosition.x = parentSize.x - pivotPosition.x + parentPos.x;
    //         relativePivotPosition.y = pivotPosition.y - parentPos.y;
    //         break;
    //     }
    //     case Anchor::Center: {
    //         relativePivotPosition.x = -(parentSize.x / 2.0f - pivotPosition.x + parentPos.x);
    //         relativePivotPosition.y = parentSize.y / 2.0f - pivotPosition.y + parentPos.y;
    //         break;
    //     }
    //     case Anchor::BottomLeft: {
    //         relativePivotPosition.x = pivotPosition.x - parentPos.x;
    //         relativePivotPosition.y = parentSize.y - pivotPosition.y + parentPos.y;
    //         break;
    //     }
    //     case Anchor::BottomRight: {
    //         relativePivotPosition.x = parentSize.x - pivotPosition.x + parentPos.x;
    //         relativePivotPosition.y = parentSize.y - pivotPosition.y + parentPos.y;
    //         break;
    //     }
    // }
}

void Widget::SetRenderOrder(int value) {
    renderOrder = value;
    parentPanel->SetDirty();
}

void Widget::UpdateTransform() {
    if (!isModelDirty || !visible) 
        return;

    glm::mat4 translation{glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x, rect.position.y, 0.0f})};
    glm::mat4 scale{glm::scale(glm::mat4{1.0f}, glm::vec3{rect.size.x, rect.size.y, 1.0f})};
    model = translation * scale;

    isModelDirty = false;
}

void Widget::AdjustPositionToAnchor(const glm::vec2& position) {
    glm::vec2 offset = pivot * rect.size;
    auto& parentPos {parentPanel->GetRect().position};
    auto& parentSize{parentPanel->GetRect().size};

    switch (anchor) {
        case Anchor::TopLeft: {
            pivotPosition = position;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::TopRight: {
            pivotPosition.x = parentSize.x - position.x;
            pivotPosition.y = position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::Center: {
            pivotPosition.x = parentSize.x / 2.0f + position.x;
            pivotPosition.y = parentSize.y / 2.0f - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::BottomLeft: {
            pivotPosition.x = position.x;
            pivotPosition.y = parentSize.y - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::BottomRight: {
            pivotPosition.x = parentSize.x - position.x;
            pivotPosition.y = parentSize.y - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
    }
}

void Widget::Draw() {
    auto uiShader{AssetManager::GetShader("gui")};
    // uiShader->Use();
    auto missingTex{AssetManager::GetTexture("default")};
    Sprite tempSprite{missingTex};
    missingTex->Use();

    UpdateTransform();
    uiShader->SetMatrix4("model", model);
    uiShader->SetVec2("spriteMinUV", tempSprite.GetMinUV());
    uiShader->SetVec2("spriteMaxUV", tempSprite.GetMaxUV());
    uiShader->SetVec4("color", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    uiShader->SetBool("flip", false);
    uiShader->SetBool("useVirtualResolution", true);
    // auto vao {AssetManager::GetVertexArray("sprite")};
    // vao->Use();
    // vao->Draw();
    AssetManager::GetVertexArray("sprite")->Draw();
}

void Widget::CalculateRelativePivotPosition() {
    auto& parentPos{parentPanel->GetRect().position};
    auto& parentSize{parentPanel->GetRect().size};

    switch (anchor) {
        case Anchor::TopLeft: {
            relativePivotPosition = pivotPosition - parentPos;
            break;
        }
        case Anchor::TopRight: {
            relativePivotPosition.x = parentSize.x - pivotPosition.x + parentPos.x;
            relativePivotPosition.y = pivotPosition.y - parentPos.y;
            break;
        }
        case Anchor::Center: {
            relativePivotPosition.x = -(parentSize.x / 2.0f - pivotPosition.x + parentPos.x);
            relativePivotPosition.y = parentSize.y / 2.0f - pivotPosition.y + parentPos.y;
            break;
        }
        case Anchor::BottomLeft: {
            relativePivotPosition.x = pivotPosition.x - parentPos.x;
            relativePivotPosition.y = parentSize.y - pivotPosition.y + parentPos.y;
            break;
        }
        case Anchor::BottomRight: {
            relativePivotPosition.x = parentSize.x - pivotPosition.x + parentPos.x;
            relativePivotPosition.y = parentSize.y - pivotPosition.y + parentPos.y;
            break;
        }
    }
}