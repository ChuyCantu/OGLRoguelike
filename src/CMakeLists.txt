target_sources(${PROJECT_NAME}
PRIVATE
    Core/AssetManager.cpp
    Core/Components.cpp
    Core/Engine.cpp
    Core/GameObject.cpp
    Core/Log.cpp
    Core/Scene.cpp
    Core/Scene.cpp
    Core/Tilemaps/Tile.cpp
    Core/Tilemaps/Tilemap.cpp
    Core/Time.cpp

    Game/Action.cpp
    Game/AI/StateMachine.cpp
    Game/AI/States/IdleState.cpp
    Game/AI/States/WanderState.cpp
    Game/Algorithms.cpp
    Game/DungeonGen/Dungeon.cpp
    Game/Enemies/Enemy.cpp
    Game/Player/Player.cpp
    Game/Scenes/Level.cpp
    Game/AutotilesLoaders.cpp
    Game/TurnManager.cpp
    
    Game/Tests/PlayerTest.cpp
    Game/Tests/Battlers.cpp
    Game/Tests/TestScene.cpp
    Game/Tests/TestScene2.cpp

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