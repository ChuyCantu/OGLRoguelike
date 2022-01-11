#include "Components.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Transform::SetPosition(const glm::vec3& position) {
    this->position = position;
    isDirty = true;
}

void Transform::SetRotation(const glm::quat& rotation) {
    this->rotation = rotation;
    isDirty = true;
}

void Transform::SetScale(const glm::vec3& scale) {
    this->scale = scale;
    isDirty = true;
}

void Transform::UpdateTransform() {
    if (isDirty) {
        // model = glm::translate(model, position);
        // model = model * glm::mat4_cast(rotation);
        // model = glm::scale(model, scale);
        glm::mat4 translation {glm::translate(glm::mat4{1.0}, position)};
        glm::mat4 rotation    {glm::mat4_cast(this->rotation)};
        glm::mat4 scale       {glm::scale(glm::mat4{1.0f}, this->scale)};
        model = translation * rotation * scale;
    }
}