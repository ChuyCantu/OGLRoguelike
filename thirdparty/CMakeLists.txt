# Dependencies: ================
# SDL2:
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)

# GLAD
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

# ImGui (included in source)
add_subdirectory(imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
target_link_libraries(imgui PRIVATE SDL2::SDL2 SDL2::SDL2main)

# GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

# FreeType 2.11.1
add_subdirectory(freetype)
# find_package(freetype CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE freetype)

# {fmt}
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# spdlog (included in source)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include)

# stb_image
add_subdirectory(stb_image)
target_link_libraries(${PROJECT_NAME} PRIVATE stb_image)

# entt
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE $ENV{DEV}/entt/src)
target_link_libraries(${PROJECT_NAME} PRIVATE entt)

# delaunator (included in source)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/delaunator/include)