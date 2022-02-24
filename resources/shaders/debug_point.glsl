#shader vertex
#version 450 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec3 Color;

layout (std140, binding = 0) uniform Globals {
    ivec2 screenSize;
    ivec2 virtualScreenSize;
    mat4 projection;
    mat4 view;
    mat4 projView;
};

out vec3 color;

void main() {
    color = Color;

    gl_Position = projView * vec4(Position, 0.0, 1.0);
}

#shader fragment
#version 450 core

out vec4 fColor;

in vec3 color;

void main() {
    fColor = vec4(color, 1.0);
}