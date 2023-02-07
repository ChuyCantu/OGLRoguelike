# stb (submodule)
add_library(stb
    stb_image.cpp
    stb/stb_image.h
    stb/stb_image_resize.h
    stb/stb_image_write.h
)
target_include_directories(stb PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/stb)

# delaunator-cpp (submodule)
add_library(delaunator INTERFACE)
target_include_directories(delaunator INTERFACE ${CMAKE_SOURCE_DIR}/thirdparty/delaunator-cpp/include)