# OGLRoguelike

Small roguelike made from scratch using OpenGL


# Building

## 1. Download git submodules:

    git submodule update --init --recursive

## 2. Bootstrap vcpkg

    cd extern/vcpkg
    bootstrap-vcpkg.bat -disableMetrics
   
## 4. Run CMake

    cmake -S . -B build
    cmake --build build