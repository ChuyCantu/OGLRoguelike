cmake_minimum_required(VERSION 3.22.0)

project(OGLRoguelike VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Logging level (options are TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL, OFF)
set(LOG_LEVEL "TRACE")

# Output directories
# https://stackoverflow.com/questions/6594796/how-do-i-make-cmake-output-into-a-bin-dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/bin)

add_executable(${PROJECT_NAME} src/main.cpp)

include_directories(src)
add_subdirectory(src)
add_subdirectory(thirdparty)

# Extra compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE "LOG_LEVEL_${LOG_LEVEL}")
#target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

# Custom commands: =============
# File managment
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

# Copy freetype dll
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/thirdparty/freetype/bin/$<CONFIGURATION>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)