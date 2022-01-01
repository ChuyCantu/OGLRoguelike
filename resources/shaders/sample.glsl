#shader vertex
#version 450 core

layout (location = 0) in vec2 ipos;
layout (location = 1) in vec3 icolor;

out vec3 color;

void main() {
    color = icolor;
    gl_Position = vec4(ipos, 0.0, 1.0);
}

#shader fragment
#version 450 core

in vec3 color;

out vec4 fColor;

void main() {
    fColor = vec4(color, 1.0);
}
