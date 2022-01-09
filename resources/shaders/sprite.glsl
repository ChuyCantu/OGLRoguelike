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

uniform mat4 model;
uniform vec2 spriteMinUV;
uniform vec2 spriteMaxUV;
uniform ivec2 spriteSize;

void main() {
    float offset = 0.00001;     

    // For triangle strip, 4 vertices, in order down-left, top-left, down-right, top-right
    switch (gl_VertexID) {
        case 0:
            texCoord = vec2(spriteMinUV.x + offset, spriteMinUV.y + offset);
            break;
        case 1:
            texCoord = vec2(spriteMinUV.x + offset, spriteMaxUV.y - offset);
            break;
        case 2:
            texCoord = vec2(spriteMaxUV.x - offset, spriteMinUV.y + offset);
            break;
        case 3:
            texCoord = vec2(spriteMaxUV.x - offset, spriteMaxUV.y - offset);
            break;
    }

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