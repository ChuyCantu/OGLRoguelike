#shader vertex
#version 450 core

layout (location = 0) in vec2 pos;

layout (std140, binding = 0) uniform Globals {
    ivec2 screenSize;
    ivec2 virtualScreenSize;
    mat4 projection;
    mat4 view;
    mat4 projView;
};

out vec2 texCoord;

void main() {


    gl_Position = projView * model * vec4(pos.x * spriteSize.x, pos.y * spriteSize.y, 0, 1);
}

#shader fragment
#version 450 core

out vec4 fColor;

in vec2 texCoord;

uniform sampler2D tex;
uniform vec4 color;

void main() {
    fColor = texture(tex, texCoord) * color;
}