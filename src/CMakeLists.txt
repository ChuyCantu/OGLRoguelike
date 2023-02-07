add_executable(${PROJECT_NAME} main.cpp)
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     CXX_STANDARD_REQUIRED ON
#     CXX_EXTENSIONS OFF
# )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

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
    Game/AI/States/ChaseState.cpp
    Game/AI/States/IdleState.cpp
    Game/AI/States/WanderState.cpp
    Game/Algorithms.cpp
    Game/AutotilesLoaders.cpp
    Game/DungeonGen/Dungeon.cpp
    Game/DungeonGen/Fov.cpp
    Game/DungeonGen/PermissiveFov.cpp
    Game/Enemies/Enemy.cpp
    Game/Player/Player.cpp
    Game/Scenes/Level.cpp
    Game/TurnManager.cpp
    Game/UnitUtils.cpp
    
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

target_link_libraries(${PROJECT_NAME} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    glad::glad
    glm::glm
    freetype
    fmt::fmt
    imgui::imgui
    spdlog::spdlog
    stb
    delaunator
)

# Extra compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE "LOG_LEVEL_${LOG_LEVEL}")

# Custom commands: =============
# File managment
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)