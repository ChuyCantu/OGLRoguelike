add_library(imgui
    imconfig.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_impl_opengl3_loader.h
    imgui_impl_opengl3.cpp
    imgui_impl_opengl3.h
    imgui_impl_sdl.cpp
    imgui_impl_sdl.h
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    imgui.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})