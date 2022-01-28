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
uniform bool flipX;
uniform bool flipY;
uniform vec2 pivot;

void main() {
    float offset = 0.00001;     

    vec2 minUV;
    vec2 maxUV;

    if (!flipX) {
        minUV.x = spriteMinUV.x + offset;
        maxUV.x = spriteMaxUV.x - offset;
    }
    else {
        minUV.x = spriteMaxUV.x - offset;
        maxUV.x = spriteMinUV.x + offset;
    }
    if (!flipY) {
        minUV.y = 1.0 - spriteMaxUV.y - offset;
        maxUV.y = 1.0 - spriteMinUV.y + offset;
    }
    else {
        minUV.y = 1.0 - spriteMinUV.y + offset;
        maxUV.y = 1.0 - spriteMaxUV.y - offset;
    }

    // For triangle strip!
    switch (gl_VertexID) {
        case 0: // Bottom-left
            texCoord = vec2(minUV.x, minUV.y);
            break;
        case 1: // Bottom-rigth
            texCoord = vec2(maxUV.x, minUV.y);
            break;
        case 2: // Top-left
            texCoord = vec2(minUV.x, maxUV.y);
            break;
        case 3: // Top-right
            texCoord = vec2(maxUV.x, maxUV.y);
            break;
    }
    
    vec2 pivotOffset = pivot * spriteSize;

    gl_Position = projView * model * vec4(pos.x * spriteSize.x - pivotOffset.x, pos.y * spriteSize.y - pivotOffset.y, 0, 1);
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