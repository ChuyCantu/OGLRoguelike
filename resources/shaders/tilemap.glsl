#shader vertex
#version 450 core 

layout (location = 0) in uint itileId;

uniform ivec2 mapSize;
uniform mat4 model;
uniform int tileSize;

out SData {
    uint tileId;
} vsOut;

void main() {
    vsOut.tileId = itileId;

    int i = gl_VertexID;
    float x = i % mapSize.x * tileSize;
    float y = i / mapSize.x * tileSize;
    gl_Position = vec4(x, y, 0, 1);
}

#shader geometry
#version 450 core 

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140, binding = 0) uniform Globals {
    ivec2 screenSize;
    ivec2 virtualScreenSize;
    mat4 projection;
    mat4 view;
    mat4 projView;
};

uniform int tileSize;
uniform mat4 model;
uniform ivec2 atlasTexSize;

out vec2 texCoord;

in SData {
    uint tileId;
} gsIn[];

out SData {
    flat uint tileId;
} gsOut;

void main() {
    uint tileId = gsIn[0].tileId - 1; // - 1 since number 0 is being considered as an empty tile
    gsOut.tileId = gsIn[0].tileId;
    vec2 tilePos = vec2((tileId % atlasTexSize.x), 
                        (tileId / atlasTexSize.x));
    vec2 uvStep = vec2(1.0 / float(atlasTexSize.x), 
                       1.0 / float(atlasTexSize.y));
    float offset = 0.00001;                       

    // Quad vertices:

    // down left
    gl_Position = projView * model * (gl_in[0].gl_Position);
    texCoord = vec2(tilePos.x * uvStep.x + offset, 1 - uvStep.y - tilePos.y * uvStep.y + offset);
    
    EmitVertex();

    // top left
    gl_Position = projView * model * (gl_in[0].gl_Position + vec4(0, tileSize, 0, 0));
    texCoord = vec2(tilePos.x * uvStep.x + offset, 1 - tilePos.y * uvStep.y - offset);
    EmitVertex();

    // down right
    gl_Position = projView * model * (gl_in[0].gl_Position + vec4(tileSize, 0, 0, 0));
    texCoord = vec2(tilePos.x * uvStep.x + uvStep.x - offset, 1 - uvStep.y - tilePos.y * uvStep.y + offset);
    EmitVertex();

    // up right
    gl_Position = projView * model * (gl_in[0].gl_Position + vec4(tileSize, tileSize, 0, 0));
    texCoord = vec2(tilePos.x * uvStep.x + uvStep.x - offset, 1 - tilePos.y * uvStep.y - offset);
    EmitVertex();

    EndPrimitive();
}

#shader fragment
#version 450 core 

in vec2 texCoord;
out vec4 fColor;

uniform sampler2D tex;

in SData {
    flat uint tileId;
} fsIn;

void main() {
    if (fsIn.tileId != 0) {
        fColor = texture(tex, texCoord);
    }
    else {
        discard;
    }
}