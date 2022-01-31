target_sources(${PROJECT_NAME}
PRIVATE
    Core/AssetManager.cpp
    Core/Components.cpp
    Core/Engine.cpp
    Core/GameObject.cpp
    Core/Log.cpp
    Core/Scene.cpp
    Core/Scene.cpp
    Core/Time.cpp

    Game/Action.cpp
    Game/Battlers.cpp
    Game/PlayerTest.cpp
    Game/TestScene.cpp
    Game/TilemapTest.cpp
    Game/TurnManager.cpp

    Input/Input.cpp
    Input/InputSystem.cpp

    Rendering/Batch.cpp
    Rendering/Buffer.cpp
    Rendering/Camera.cpp
    Rendering/Framebuffer.cpp
    Rendering/Renderer.cpp
    Rendering/Shader.cpp
    Rendering/Sprite.cpp
    Rendering/Texture.cpp
    Rendering/UniformBuffer.cpp
    Rendering/VertexArray.cpp

    UI/Image.cpp
    UI/Panel.cpp
    UI/Rect.cpp
    UI/Widget.cpp
    UI/UIStack.cpp

    Utils/MathExtras.cpp
    Utils/OGLDebug.cpp
    Utils/Random.cpp
)