target_sources(${PROJECT_NAME}
PRIVATE
    Core/AssetsManager.cpp
    Core/Engine.cpp
    Core/Log.cpp
    Core/Scene.cpp
    Core/Time.cpp

    Input/Input.cpp
    Input/InputSystem.cpp

    Rendering/Buffer.cpp
    Rendering/Camera.cpp
    Rendering/Framebuffer.cpp
    Rendering/Renderer.cpp
    Rendering/Shader.cpp
    Rendering/Sprite.cpp
    Rendering/Texture.cpp
    Rendering/TilemapRenderer.cpp
    Rendering/UniformBuffer.cpp
    Rendering/VertexArray.cpp

    Utils/OGLDebug.cpp
    Utils/Random.cpp
)