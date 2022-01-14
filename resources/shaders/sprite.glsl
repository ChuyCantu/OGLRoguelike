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
uniform bool flip;

void main() {
    float offset = 0.00001;     

    // For triangle strip
    if (!flip) {
        switch (gl_VertexID) {
            case 0: // Down-left
                texCoord = vec2(spriteMinUV.x + offset, spriteMinUV.y + offset);
                break;
            case 1: // Down-rigth
                texCoord = vec2(spriteMaxUV.x - offset, spriteMinUV.y + offset);
                break;
            case 2: // Top-left
                texCoord = vec2(spriteMinUV.x + offset, spriteMaxUV.y - offset);
                break;
            case 3: // Top-right
                texCoord = vec2(spriteMaxUV.x - offset, spriteMaxUV.y - offset);
                break;
        }
    }
    else {
        switch (gl_VertexID) {
            case 0: // Down-left (now rigth)
                texCoord = vec2(spriteMaxUV.x - offset, spriteMinUV.y + offset);
                break;
            case 1: // Down-rigth (now left)
                texCoord = vec2(spriteMinUV.x + offset, spriteMinUV.y + offset);
                break;
            case 2: // Top-left (now right)
                texCoord = vec2(spriteMaxUV.x - offset, spriteMaxUV.y - offset);
                break;
            case 3: // Top-right (now left)
                texCoord = vec2(spriteMinUV.x + offset, spriteMaxUV.y - offset);
                break;
        }
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