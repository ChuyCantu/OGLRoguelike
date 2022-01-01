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

uniform float test1;
uniform vec2 test2;
uniform float[2] test3;
uniform bool test4;

void main() {
    if (test4)
        fColor = vec4(color * test1 * test2.x * test3[0] * test3[1], 1.0);
    else
        fColor = vec4(color, 1.0);
}
