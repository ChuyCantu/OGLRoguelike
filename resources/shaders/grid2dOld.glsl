#shader vertex
#version 450 core

layout (location = 0) in vec3 pos;

void main() {
    gl_Position = vec4(pos, 1.0);
}

#shader fragment
#version 450 core

layout (std140, binding = 0) uniform Globals {
    ivec2 screenSize;
    ivec2 virtualScreenSize;
    mat4 projection;
    mat4 view;
    mat4 projView;
};

uniform ivec2 tileSize;
uniform vec3 cameraPos;
out vec4 fColor;

bool NearZero(float f, float tolerance) {
    return f > -tolerance && f < tolerance;
}

void main() {
    vec2 aspectRatio = virtualScreenSize / vec2(screenSize);
    vec2 gridOffset = cameraPos.xy / aspectRatio; // scaled from virtual screen pos to screen pos
    vec2 scaledTileSize = tileSize / aspectRatio;
    vec2 screenCenter = screenSize / 2.0;
   
    vec2 fragWorldPos = vec2(-screenCenter + gl_FragCoord.xy + gridOffset);
    vec2 fragPos = vec2(-screenCenter + gl_FragCoord.xy);

    if (NearZero(fract(fragPos.x / scaledTileSize.x), 0.05) || NearZero(fract(fragPos.y / scaledTileSize.y), 0.05)) {
        if (int(fragWorldPos.x) == 0) 
        fColor = vec4(1.0, 0.0, 0.0, 0.2);  
        else
            fColor = vec4(1.0, 1.0, 1.0, 0.1);
    }
    if (int(fragWorldPos.y) == 0) {
        fColor = vec4(0.0, 1.0, 0.0, 0.2);
    }   
}

// This grid renderer is just meant for debugging and is bad implemented since it's not meant to be used in the final game