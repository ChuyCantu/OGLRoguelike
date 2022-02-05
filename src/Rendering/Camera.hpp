#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Common.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

class Camera {
public:
    Camera(const glm::ivec2& virtualSize, class Renderer* renderer);

    void SetPosition(const glm::vec3 position);
    void SetScale(float scale);
    void SetVirtualSize(const glm::ivec2 size);
    glm::vec2 ScreenToWorld2D(const glm::vec2& point);
    glm::vec2 World2DToScreen(const glm::vec2& point);
    glm::vec2 World2DToVirtualScreen(const glm::vec2& point);
    bool IsPointInside(const glm::vec2& point);

    glm::vec2 GetScreenVsVirtualSizeScaleRatio() const;

    const glm::vec3& GetPosition() const { return position; }
    float GetScale() const { return scale; }
    const glm::ivec2 GetVirtualSize() const { return virtualSize; }
    bool IsMainCamera() const { return isMainCamera; }

    static void SetMainCamera(Ref<Camera> camera);
    static Camera& GetMainCamera() { return *mainCamera; }

private:
    void UpdateProjection();
    void UpdateViewMatrix();

private:
    bool isMainCamera  {false};
    glm::vec3 position {0.f};
    glm::vec3 up       {0.0f, 1.0f, 0.0f};
    glm::vec3 forward  {0.0f, 0.0f, -1.0f};
    float scale        {1.0f};
    glm::ivec2 virtualSize;
    glm::mat4 projection;
    glm::mat4 view;
    class Renderer* renderer;
    
    static Ref<Camera> mainCamera;
};

#endif // __CAMERA_H__