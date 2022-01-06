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

// layout (std140, binding = 0) uniform Globals {
//     ivec2 screenSize;
//     ivec2 virtualScreenSize;
//     mat4 projection;
// };

layout (std140, binding = 0) uniform Globals {
    ivec2 screenSize;
    ivec2 virtualScreenSize;
    mat4 projection;
    mat4 view;
    mat4 projView;
};

uniform mat4 model;
// uniform mat4 view;
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
    const float tileSizeMid = tileSize / 2.0;
    const float offset = 0.0001; // 1 / 256.0;

    vec4 pos = gl_in[0].gl_Position;

    // down left
    gl_Position = projView * model * vec4(pos.x * tileSize, pos.y * tileSize, pos.zw);
    texCoord = vec2(tileX + offset, 1.0 - tileUVSize - tileY + offset);
    EmitVertex();

    // top left
    gl_Position = projView * model * vec4(pos.x * tileSize, tileSize * (pos.y + 1.0), pos.zw);
    texCoord = vec2(tileX + offset, 1.0 - tileY - offset);
    EmitVertex();

    // down right
    gl_Position = projView * model * vec4(tileSize * (pos.x + 1.0), pos.y * tileSize, pos.zw);
    texCoord = vec2(tileUVSize + tileX - offset, 1.0 - tileUVSize - tileY + offset);
    EmitVertex();

    // up right
    gl_Position = projView * model * vec4(tileSize * (pos.x + 1.0), tileSize * (pos.y + 1.0), pos.zw);
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