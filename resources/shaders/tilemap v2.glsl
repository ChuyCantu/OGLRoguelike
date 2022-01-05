#shader vertex
#version 450 core 

layout (location = 0) in uint itileId;

uniform vec2 mapSize;

out SData {
    uint tileId;
} vsOut;

void main() {
    vsOut.tileId = itileId;

    int i = gl_VertexID;
    float x = i / int(mapSize.y);
    float y = i % int(mapSize.y);
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
};

uniform mat4 model;
uniform vec2 mapSize;
out vec2 texCoord;

in SData {
    uint tileId;
} gsIn[];

void main() {
    uint tileId = uint(gsIn[0].tileId); // uint(someData);
    float tileX = float(tileId & 15u) / 16.0;
    float tileY = float((tileId >> 4u) & 15u) / 16.0;
    const float tileUVSize = 1 / 16.0;
    const int tileSize = 16;
    int tileWidth = virtualScreenSize.x / int(mapSize.x);
    int tileHeight = virtualScreenSize.y / int(mapSize.y);
    const float offset = 0.0001; // 1 / 256.0;

    vec4 pos = gl_in[0].gl_Position;

    // down left
    gl_Position = projection * model * vec4(pos.x * tileWidth, pos.y * tileHeight, pos.zw);
    texCoord = vec2(tileX + offset, 1.0 - tileUVSize - tileY + offset);
    EmitVertex();

    // top left
    gl_Position = projection * model * vec4(pos.x * tileWidth, tileHeight * (pos.y + 1.0), pos.zw);
    texCoord = vec2(tileX + offset, 1.0 - tileY - offset);
    EmitVertex();

    // down right
    gl_Position = projection * model * vec4(tileWidth * (pos.x + 1.0), pos.y * tileHeight, pos.zw);
    texCoord = vec2(tileUVSize + tileX - offset, 1.0 - tileUVSize - tileY + offset);
    EmitVertex();

    // up right
    gl_Position = projection * model * vec4(tileWidth * (pos.x + 1.0), tileHeight * (pos.y + 1.0), pos.zw);
    texCoord = vec2(tileUVSize + tileX - offset, 1.0 - tileY - offset);
    EmitVertex();

    EndPrimitive();
}

#shader fragment
#version 450 core 

in vec2 texCoord;
out vec4 fColor;

uniform sampler2D tex;

void main() {
    // fColor = texture(tex, texCoord);
    vec4 color = texture(tex, texCoord);
    fColor = vec4(vec3(color.g), 1.0);

    // // For coloring bg and fb:
    // vec4 color = texture(tex, texCoord);
    // vec3 finalColor = vec3(1.0);
    // if (color.g == 0) {
    //     finalColor.r = 0.2;
    //     finalColor.g = 0.1;
    //     finalColor.b = 0.4;
    // }
    // else {
    //     finalColor = vec3(color.g) * vec3(1.0, 0.0, 0.5); //vec3(0.5, 0.2, 0.0);
    // }
    // fColor = vec4(finalColor, 1.0);
}