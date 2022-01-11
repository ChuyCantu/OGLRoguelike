#ifndef __MATHCONSTANTS_H__
#define __MATHCONSTANTS_H__

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace vec2 {
    constexpr glm::vec2 zero  {0.0f, 0.0f};
    constexpr glm::vec2 up    {0.0f, 1.0f};
    constexpr glm::vec2 down  {0.0f, -1.0f};
    constexpr glm::vec2 right {1.0f, 0.0f};
    constexpr glm::vec2 left  {-1.0f, 0.0f};
    constexpr glm::vec2 inf   {std::numeric_limits<float>::infinity()};
    constexpr glm::vec2 ninf  {-std::numeric_limits<float>::infinity()};
}

namespace vec3 {
    constexpr glm::vec3 zero  {0.0f, 0.0f, 0.0f};
    constexpr glm::vec3 up    {0.0f, 1.0f, 0.0f};
    constexpr glm::vec3 down  {0.0f, -1.0f, 0.0f};
    constexpr glm::vec3 right {1.0f, 0.0f, 0.0f};
    constexpr glm::vec3 left  {-1.0f, 0.0f, 0.0f};
    constexpr glm::vec3 front {0.0f, 0.0f, 1.0f};
    constexpr glm::vec3 back  {0.0f, 0.0f, -1.0f};
    constexpr glm::vec3 inf   {std::numeric_limits<float>::infinity()};
    constexpr glm::vec3 ninf  {-std::numeric_limits<float>::infinity()};
}

namespace quaternion {
    constexpr glm::quat identity {1.0f, 0.0f, 0.0f, 0.0f};
}

struct Transform;

/**
 * @brief Rotates a transform around a pivot
 * 
 * @param transform The transform to rotate
 * @param pivot The reference point for the rotation
 * @param axis A normalized axis for the rotation
 * @param angle The angle in radians to rotate
 */
void RotateAroundPivot(Transform& transform, const glm::vec3& pivot, const glm::vec3& axis, float angle);

/**
 * @brief Rotates a transform around a pivot
 * 
 * @param transform The transform to rotate
 * @param pivot The reference point for the rotation
 * @param rot The quaternion used for the rotation
 */
void RotateAroundPivot(Transform& transform, const glm::vec3& pivot, glm::quat rot);

#endif // __MATHCONSTANTS_H__