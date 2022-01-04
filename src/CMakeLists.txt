target_sources(${PROJECT_NAME}
PRIVATE
    Core/AssetsManager.cpp
    Core/Engine.cpp
    Core/Log.cpp
    Core/Time.cpp

    Input/Input.cpp
    Input/InputSystem.cpp

    Rendering/Buffer.cpp
    Rendering/Renderer.cpp
    Rendering/Shader.cpp
    Rendering/Texture.cpp
    Rendering/TilemapRenderer.cpp
    Rendering/UniformBuffer.cpp
    Rendering/VertexArray.cpp

    Utils/OGLDebug.cpp
    Utils/Random.cpp
)