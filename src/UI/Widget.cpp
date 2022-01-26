#include "Widget.hpp"

#include "Panel.hpp"

#include <glm/gtc/matrix_transform.hpp>

Widget::Widget(Panel* panel) : parentPanel{panel} {

}

Widget::Widget(Panel* panel, const Rect& rect) : parentPanel{panel}, rect{rect} {
    
}

Widget::~Widget() {
    
}

void Widget::SetPosition(const glm::vec2& position) {
    isModelDirty = true;
    pivotPosition = position;
    rect.position = pivotPosition - pivot * rect.size;

    // Update position
   
}

void Widget::SetRelativePosition(const glm::vec2& position) {
    isModelDirty = true;
    glm::vec2 offset = pivot * rect.size;

    // Update position
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
            pivotPosition.y = parentPanel->rect.size.y / 2.0f + position.y;
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

void Widget::SetSize(const glm::vec2& size) {
    isModelDirty = true;
    rect.size = size;
}

void Widget::SetRect(const Rect& rect) {
    isModelDirty = true;
    this->rect = rect;

    // Update position
}

void Widget::SetPivot(const glm::vec2& pivot) {
    // isModelDirty = true;
    this->pivot = pivot;

    // Update position
}

void Widget::SetAnchor(Anchor anchor) {
    // isModelDirty = true;
    this->anchor = anchor;

    // Update position
    
}


void Widget::UpdateTransform() {
    if (!isModelDirty) 
        return;

    glm::mat4 translation{glm::translate(glm::mat4{1.0}, glm::vec3{rect.position.x, rect.position.y, 0.0f})};
    glm::mat4 scale{glm::scale(glm::mat4{1.0f}, glm::vec3{rect.size.x, rect.size.y, 1.0f})};
    model = translation * scale;

    isModelDirty = false;
}