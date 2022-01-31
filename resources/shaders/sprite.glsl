#shader vertex
#version 450 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;
layout (location = 3) in int TexIndex;

layout (std140, binding = 0) uniform Globals {
    ivec2 screenSize;
    ivec2 virtualScreenSize;
    mat4 projection;
    mat4 view;
    mat4 projView;
};

out vec2 texCoord;
out vec4 color;
out flat int texIndex;

void main() {
    texCoord = UV;
    color = Color;
    texIndex = TexIndex;

    gl_Position = projView * Position;
}

#shader fragment
#version 450 core

out vec4 fColor;

in vec2 texCoord;
in vec4 color;
in flat int texIndex;

uniform sampler2D textures[32];

void main() {
    fColor = texture(textures[texIndex], texCoord) * color;
}