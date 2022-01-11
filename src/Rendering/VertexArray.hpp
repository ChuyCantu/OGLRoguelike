#ifndef __VERTEXARRAY_H__
#define __VERTEXARRAY_H__

#include "Buffer.hpp"

#include <stdint.h>
#include <vector>

enum class DrawMode : uint32_t {
    Points,
    LineStrip,
    LineLoop,
    Lines,
    LineStripAdjacency,
    LinesAdjacency,
    TriangleStrip,
    TriangleFan,
    Triangles,
    TriangleStripAdjacency,
    TrianglesAdjacency,
    Patches
};

uint32_t GetOpenGLDrawMode(DrawMode drawMode);

struct VertexElement {
    uint32_t size         {0};
    DataType dataType     {DataType::Float};
    // Setting this to true will use glVertexArrayAttribIFormat instead of glVertexArrayAttribFormat. See (https://community.khronos.org/t/vertex-shader-integer-input-broken/72878) for more information.
    bool useIntegerFormat {false}; 
    bool normalized       {false};
    uint32_t offset       {0};
};

using VertexLayout = std::vector<VertexElement>;

// TODO: Improve constructor of VertexArray

class VertexArray {
public:
    VertexArray();
    VertexArray(const void* vertices, uint32_t verticesCount,
                VertexLayout& layout, BufferUsage vDrawUsage = BufferUsage::Static,
                const uint32_t* indices = nullptr, uint32_t indicesCount = 0, BufferUsage iDrawUsage = BufferUsage::Static);
    VertexArray(VertexArray&& other);
    VertexArray& operator=(VertexArray&& other);
    ~VertexArray();
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void Use() const;
    void Unbind() const;
    void Destroy();
    void Draw() const;
    bool IsNull() const { return id == 0 && vbo.IsNull(); }

    void SetDrawMode(DrawMode drawMode);

    uint32_t GetID() const { return id; }
    uint32_t GetVerticesCount() const { return verticesCount; }
    uint32_t GetIndicesCount() const { return indicesCount; }

    // TODO: Change to const?
    // uint32_t GetVertexBufferID() { return vbo.GetID(); }
    Buffer& GetVertexBuffer() { return vbo; }
    // uint32_t GetIndexBufferID() { return ibo.GetID(); }
    Buffer& GetIndexBuffer() { return ibo; }

   private:
    uint32_t id  {0};
    Buffer vbo;
    Buffer ibo;

    uint32_t verticesCount {0};
    uint32_t indicesCount {0};
    DrawMode drawMode {DrawMode::Triangles};
    
};

uint32_t CalculateVertexSizeAndOffsets(VertexLayout& layout);

#endif // __VERTEXARRAY_H__