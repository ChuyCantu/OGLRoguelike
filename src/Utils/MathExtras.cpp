#include "MathExtras.hpp"

#include "Core/Components.hpp"

void RotateAroundPivot(Transform& transform, const glm::vec3& pivot, const glm::vec3& axis, float angle) {
    glm::quat rot{glm::angleAxis(angle, axis)};
    transform.SetPosition(rot * (transform.GetPosition() - pivot) + pivot);
    transform.SetRotation(rot * transform.GetRotation());
    // Quaternion rot = Quaternion.AngleAxis(angle, axis);
    // transform.position = rot * (transform.position - pivotPoint) + pivotPoint;
    // transform.rotation = rot * transform.rotation;
}

void RotateAroundPivot(Transform& transform, const glm::vec3& pivot, glm::quat rot) {
    transform.SetPosition(rot * (transform.GetPosition() - pivot) + pivot);
    transform.SetRotation(rot * transform.GetRotation());
    // transform.position = rot * (transform.position - pivotPoint) + pivotPoint;
    // transform.rotation = rot * transform.rotation;
}

float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

glm::vec2 Lerp(const glm::vec2& a, const glm::vec2& b, float t) {
    return a + t * (b - a);
}

glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t) {
    return a + t * (b - a);
}
