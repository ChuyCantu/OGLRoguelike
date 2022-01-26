#include "Widget.hpp"

#include "Panel.hpp"

#include <glm/gtc/matrix_transform.hpp>

Widget::Widget(Panel* panel) : parentPanel{panel} { }

Widget::Widget(Panel* panel, const Rect& rect) : parentPanel{panel} {
    SetRect(rect);
}

Widget::~Widget() { }

void Widget::SetPosition(const glm::vec2& position) {
    isModelDirty = true;
    relativePivotPosition = position;

    AdjustPositionToAnchor(position);
}

void Widget::SetRelativePosition(const glm::vec2& position) {
    isModelDirty = true;
    relativePivotPosition = position;

    AdjustPositionToAnchor(position + parentPanel->rect.position);
}

void Widget::SetSize(const glm::vec2& size) {
    isModelDirty = true;
    rect.size = size;
}

void Widget::SetRect(const Rect& rect) {
    isModelDirty = true;
    relativePivotPosition = rect.position + pivot * rect.size;
    pivotPosition = relativePivotPosition;

    this->rect = rect;
}

void Widget::SetPivot(const glm::vec2& pivot) {
    pivotPosition = rect.position + pivot * rect.size;
    relativePivotPosition = relativePivotPosition + rect.size * (pivot - this->pivot);
    this->pivot = pivot;
}

void Widget::SetAnchor(Anchor anchor) {
    this->anchor = anchor;

    glm::vec2 offset = pivot * rect.size;

    switch (anchor) {
        case Anchor::TopLeft: {
            relativePivotPosition = pivotPosition;
            break;
        }
        case Anchor::TopRight: {
            relativePivotPosition.x = parentPanel->rect.size.x - pivotPosition.x;
            relativePivotPosition.y = pivotPosition.y;
            break;
        }
        case Anchor::Center: {
            relativePivotPosition.x = -(parentPanel->rect.size.x / 2.0f - pivotPosition.x);
            relativePivotPosition.y = parentPanel->rect.size.y / 2.0f - pivotPosition.y;
            break;
        }
        case Anchor::BottomLeft: {
            relativePivotPosition.x = pivotPosition.x;
            relativePivotPosition.y = parentPanel->rect.size.y - pivotPosition.y;
            break;
        }
        case Anchor::BottomRight: {
            relativePivotPosition.x = parentPanel->rect.size.x - pivotPosition.x;
            relativePivotPosition.y = parentPanel->rect.size.y - pivotPosition.y;
            break;
        }
    }
}

void Widget::UpdateTransform() {
    if (!isModelDirty || !enabled || !visible) 
        return;

    glm::mat4 translation{glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x, rect.position.y, 0.0f})};
    glm::mat4 scale{glm::scale(glm::mat4{1.0f}, glm::vec3{rect.size.x, rect.size.y, 1.0f})};
    model = translation * scale;

    isModelDirty = false;
}

void Widget::AdjustPositionToAnchor(const glm::vec2& position) {
    glm::vec2 offset = pivot * rect.size;

    switch (anchor) {
        case Anchor::TopLeft: {
            pivotPosition = position;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::TopRight: {
            pivotPosition.x = parentPanel->rect.size.x - position.x;
            pivotPosition.y = position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::Center: {
            pivotPosition.x = parentPanel->rect.size.x / 2.0f + position.x;
            pivotPosition.y = parentPanel->rect.size.y / 2.0f - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::BottomLeft: {
            pivotPosition.x = position.x;
            pivotPosition.y = parentPanel->rect.size.y - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
        case Anchor::BottomRight: {
            pivotPosition.x = parentPanel->rect.size.x - position.x;
            pivotPosition.y = parentPanel->rect.size.y - position.y;
            rect.position = pivotPosition - offset;
            break;
        }
    }
}