#pragma once

#include <glad/glad.h>

// https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_debug.txt
namespace OGLDebugOutput {
    void Enable(bool syncrhonous = false);
    void Disable();

    // Credits for this: https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#glbuffer
    void Message(GLenum source, GLenum type, GLuint id, 
                 GLenum severity, GLsizei length, 
                 GLchar const* message, void const* user_param);
}

// To disable notifications:
// glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);