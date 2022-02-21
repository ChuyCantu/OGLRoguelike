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
    Game/TestScene2.cpp
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

    UI/Button.cpp
    UI/Checkbox.cpp
    UI/Image.cpp
    UI/Label.cpp
    UI/Panel.cpp
    UI/Rect.cpp
    UI/Text/TextRenderer.cpp
    UI/ScrollView.cpp
    UI/Slider.cpp
    UI/UI.cpp
    UI/UIStack.cpp
    UI/Widget.cpp

    Utils/Color.cpp
    Utils/MathExtras.cpp
    Utils/OGLDebug.cpp
    Utils/Random.cpp
)