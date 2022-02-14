#shader vertex
#version 450 core

layout (location = 0) in vec2 pos;

layout (std140, binding = 1) uniform UIMatrices {
    mat4 uiProjection;
    mat4 uiVirtualProjection;
};

out vec2 texCoord;

uniform mat4 model;
uniform vec2 spriteMinUV;
uniform vec2 spriteMaxUV;
uniform bool flipX;
uniform bool flipY;
uniform bool useVirtualResolution;

void main() {
    float offset = 0.00001;     

    // NOTE: UVs are inverted in comparition to the sprite shader
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
        minUV.y = 1.0 - spriteMinUV.y + offset;
        maxUV.y = 1.0 - spriteMaxUV.y - offset;
    }
    else {
        minUV.y = 1.0 - spriteMaxUV.y - offset;
        maxUV.y = 1.0 - spriteMinUV.y + offset;
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

    if (useVirtualResolution) {
        gl_Position = uiVirtualProjection * model * vec4(pos.x, pos.y, 0, 1);
    }
    else {
        gl_Position = uiProjection * model * vec4(pos.x, pos.y, 0, 1);
    }
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