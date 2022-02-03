#shader vertex
#version 450 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec4 color;

layout (std140, binding = 1) uniform UIMatrices {
    mat4 uiProjection;
    mat4 uiVirtualProjection;
};

layout (std140, binding = 0) uniform Globals {
    ivec2 screenSize;
    ivec2 virtualScreenSize;
    mat4 projection;
    mat4 view;
    mat4 projView;
};

out vec2 texCoords;
out vec4 textColor;

void main() {
    gl_Position = uiVirtualProjection * vec4(vertex.xy, 0.0, 1.0);
    texCoords = vertex.zw;
    textColor = color;
}

#shader fragment
#version 450 core

struct TextInfo {
    // vec4 color;
    float width;
    float edge;
    float borderWidth; // Setting to 0 make the border disappear
    float borderEdge;
    vec2 borderOffset; // For drop shadows
    vec3 outlineColor;
};

out vec4 fragColor;

in vec2 texCoords;
in vec4 textColor;

uniform sampler2D tex;
uniform TextInfo textInfo;

// const float width = 0.45; 
// const float edge = 0.1;

// const float borderWidth = 0.6;
// const float borderEdge = 0.1;

// const vec2 borderOffset = vec2(0.05, 0.04);  // For drop shadows

// const vec3 outlineColor = vec3(1.0, 0.0, 0.0);

void main() {
    float distance = 1.0 - texture(tex, texCoords).r;
    float alpha = 1.0 - smoothstep(textInfo.width, textInfo.width + textInfo.edge, distance);

    float distance2 = 1.0 - texture(tex, texCoords + textInfo.borderOffset).r;
    float outlineAlpha = 1.0 - smoothstep(textInfo.borderWidth, textInfo.borderWidth + textInfo.borderEdge, distance2);

    float overallAlpha = alpha + (1.0 - alpha) * outlineAlpha;
    vec3 overallColor = mix(textInfo.outlineColor, textColor.rgb, alpha / overallAlpha);

    fragColor = vec4(overallColor, overallAlpha);
}