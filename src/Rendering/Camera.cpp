#include "Camera.hpp"

#include "Core/AssetManager.hpp"
#include "Core/Log.hpp"
#include "Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Ref<Camera> Camera::mainCamera;

Camera::Camera(const glm::ivec2& virtualSize, class Renderer* renderer)
    : virtualSize{virtualSize}, renderer{renderer} {
    UpdateProjection();
    UpdateViewMatrix();
}

void Camera::SetPosition(const glm::vec3 position) {
    this->position = position;
    UpdateViewMatrix();
}

void Camera::SetScale(float scale) {
    this->scale = scale;
    UpdateProjection();
}

// TODO: Trigger event
void Camera::SetVirtualSize(const glm::ivec2 size) {
    virtualSize = size;
    UpdateProjection();

    if (isMainCamera) {
        Ref<Buffer> buffer{AssetManager::GetBuffer("Globals")};
        if (buffer) {
            buffer->SetData(8, sizeof(glm::mat4), glm::value_ptr(virtualSize));
        }
    }
}

glm::vec2 Camera::ScreenToWorld2D(const glm::vec2& point) {
    //+ For camera origin (0, 0) at the center of the screen and screen origin at top-left corner:
    //+ Positive axes: camera/world (right, up), screen (right, down)
    float widthFactor {float(virtualSize.x / scale) / float(renderer->screenSize.x)};
    float heightFactor {float(virtualSize.y / scale) / float(renderer->screenSize.y)};

    glm::vec2 screenPos = point * glm::vec2{widthFactor, heightFactor};
    glm::vec2 screenCenter {virtualSize.x / scale / 2.0f, virtualSize.y / scale / 2.0f};

    glm::vec2 worldPos {screenPos.x - screenCenter.x + position.x, screenCenter.y - screenPos.y + position.y};

    // LOG_TRACE("Screen x: {}, y: {}.\nVirtual x: {}, y: {}.\nWorld x: {}, y: {}.\n", point.x, point.y, screenPos.x, screenPos.y, worldPos.x, worldPos.y);

    return worldPos;
}

glm::vec2 Camera::World2DToScreen(const glm::vec2& point) {
    // LOG_TRACE("nWorld x: {}, y: {}.\n", (point - glm::vec2{position.x, position.y}).x, (point - glm::vec2{position.x, position.y}).y);
    return point - glm::vec2{position.x, position.y};
}

glm::vec2 Camera::World2DToVirtualScreen(const glm::vec2& point) {
    // LOG_TRACE("nWorld x: {}, y: {}.", ((point - glm::vec2{position.x, position.y}) / scale).x, ((point - glm::vec2{position.x, position.y}) / scale).y);
    return (point - glm::vec2{position.x, position.y}) / scale;
}

bool Camera::IsPointInside(const glm::vec2& point) {
    glm::vec2 cameraExtends {virtualSize.x / scale / 2.0f, virtualSize.y / scale / 2.0f};
    return point.x >= -cameraExtends.x && point.x <= cameraExtends.x 
        && point.y >= -cameraExtends.y && point.y <= cameraExtends.y;
}

glm::vec2 Camera::GetScreenVsVirtualSizeScaleRatio() const {
    return glm::vec2{(float)virtualSize.x / renderer->screenSize.x, (float)virtualSize.y / renderer->screenSize.y};
}

void Camera::SetMainCamera(Ref<Camera> camera) {
    if (mainCamera)
        mainCamera->isMainCamera = false;

    camera->isMainCamera = true;
    mainCamera = camera;

    mainCamera->UpdateProjection();
    mainCamera->UpdateViewMatrix();

    Ref<Buffer> buffer{AssetManager::GetBuffer("Globals")};
    if (buffer) {
        buffer->SetData(8, sizeof(glm::mat4), glm::value_ptr(camera->virtualSize));
    }
}

void Camera::UpdateProjection() {
    // May add a projection type member and update the projection to ortho or perspective based on that

    projection = glm::ortho(-(float)virtualSize.x / 2.0f / scale,
                             (float)virtualSize.x / 2.0f / scale, 
                            -(float)virtualSize.y / 2.0f / scale, 
                             (float)virtualSize.y / 2.0f / scale);

    if (isMainCamera) {
        Ref<Buffer> buffer{AssetManager::GetBuffer("Globals")};
        if (buffer) {
            buffer->SetData(16, sizeof(glm::mat4), glm::value_ptr(projection));

            glm::mat4 projView {projection * view};
            buffer->SetData(144, sizeof(glm::mat4), glm::value_ptr(projView));
        }
        Ref<Buffer> uiBuffer{AssetManager::GetBuffer("UIMatrices")};
        if (uiBuffer) {
            auto uiVirtualProjection{glm::ortho(0.f,
                                         static_cast<float>(virtualSize.x),
                                         static_cast<float>(virtualSize.y),
                                         0.f)};
            uiBuffer->SetData(64, sizeof(glm::mat4), glm::value_ptr(uiVirtualProjection));
        }
    }
}

void Camera::UpdateViewMatrix() {
    // position = glm::vec3{int(position.x), int(position.y), int(position.z)};
    view = glm::lookAt(position, position + forward, up);

    if (isMainCamera) {
        Ref<Buffer> buffer{AssetManager::GetBuffer("Globals")};
        if (buffer) {
            buffer->SetData(80, sizeof(glm::mat4), glm::value_ptr(view));

            glm::mat4 projView {projection * view};
            buffer->SetData(144, sizeof(glm::mat4), glm::value_ptr(projView));
        }
    }
}

//+ Globals UniformBuffer binding = 0:
//+ Loaded in Renderer.cpp
//*                           size   |  offset
//* vec2 screenSize        - 8  (2N)      0
//* vec2 virtualScreenSize - 8  (2N)      8
//* mat4 projection        - 64 (16N)     16
//* mat4 view              - 64 (16N)     80
//* mat4 projView          - 64 (16N)     144
//*                        - 208          