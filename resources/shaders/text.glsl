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

out vec4 textColor;
out vec2 texCoords;

void main() {
    gl_Position = uiVirtualProjection * vec4(vertex.xy, 0.0, 1.0);
    textColor = color;
    texCoords = vertex.zw;
}

#shader fragment
#version 450 core

out vec4 fragColor;

in vec4 textColor;
in vec2 texCoords;

uniform sampler2D text;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoords).r);
    fragColor = textColor * sampled;
}