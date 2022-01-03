#shader vertex
#version 450 core 

layout (location = 0) in uint itileId;

uniform vec2 mapSize;
uniform mat4 projection;

out SData {
    uint tileId;
} vsOut;

void main() {
    // vsOut.tileId = gl_VertexID % 4;
    vsOut.tileId = itileId;

    int i = gl_VertexID;
    float x = i / int(mapSize.y);
    float y = i % int(mapSize.y);
    // gl_Position = projection * vec4(x * 16 + 50, y * 16 + 20, 0, 1);
    gl_Position = vec4(x, y, 0, 1);
}

#shader geometry
#version 450 core 

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 projection;

in SData {
    uint tileId;
} gsIn[];

out vec3 color;
out vec2 texCoord;

void main() {
    uint tileId = gsIn[0].tileId & 225u;
    float tileX = float(tileId & 15u) / 16.0;
    float tileY = float((tileId >> 4u) & 15u) / 16.0;

    const float B = 1 / 256.0;
    const float S = 1 / 16.0;

    int tileSizeMid = 16 / 2;
    float offset = float(gsIn[0].tileId) * (1.0 / 16.0);

    vec4 pos = gl_in[0].gl_Position;

    // gl_Position = projection * vec4(pos.x * 16 + 50, pos.y * 16 + 20, pos.zw);
    // color = vec3(1.0);
    // EmitVertex();

    // down left
    gl_Position = projection * vec4(pos.x * 16 + 50 - tileSizeMid, pos.y * 16 + 20 - tileSizeMid, pos.zw);
    // texCoord = vec2(0.0 + offset, 1.0);
    texCoord = vec2(0.0 + offset, 1.0 - (1.0 / 16.0));
    color = vec3(1.0, 0.0, 0.0);
    EmitVertex();

    // top left
    gl_Position = projection * vec4(pos.x * 16 + 50 - tileSizeMid, pos.y * 16 + 20 + tileSizeMid, pos.zw);
    // texCoord = vec2(0.0 + offset, 1.0 - (1.0 / 16.0));
    texCoord = vec2(0.0 + offset, 1.0);
    color = vec3(1.0, 0.0, 0.0);
    EmitVertex();

    // down right
    gl_Position = projection * vec4(pos.x * 16 + 50 + tileSizeMid, pos.y * 16 + 20 - tileSizeMid, pos.zw);
    // texCoord = vec2(1.0 / 16.0 + offset, 1.0 - (1.0 / 16.0));
    texCoord = vec2(1.0 / 16.0 + offset, 1.0 - (1.0 / 16.0));
    color = vec3(1.0, 0.0, 0.0);
    EmitVertex();

    // up right
    gl_Position = projection * vec4(pos.x * 16 + 50 + tileSizeMid, pos.y * 16 + 20 + tileSizeMid, pos.zw);
    // texCoord = vec2(1.0 / 16.0 + offset, 1.0);
    texCoord = vec2(1.0 / 16.0 + offset, 1.0);
    color = vec3(1.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}

#shader fragment
#version 450 core 

in vec3 color;
in vec2 texCoord;
out vec4 fColor;

uniform sampler2D tex;

void main() {
    // fColor = vec4(color, 1.0);
    fColor = vec4(vec3(texture(tex, texCoord).g), 1.0);
    // fColor = texture(tex, texCoord);
}