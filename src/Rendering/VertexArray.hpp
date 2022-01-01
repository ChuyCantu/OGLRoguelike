#ifndef __VERTEXARRAY_H__
#define __VERTEXARRAY_H__

#include <stdint.h>
#include <vector>

enum class DataType : uint32_t {
    Bool,
    Byte,
    UByte,
    Int,
    Uint,
    Float,
    Double
};

uint32_t GetOpenGLDataType(DataType dataType);
uint32_t GetDataTypeSize(DataType dataType);

enum class DrawUsage : uint32_t {
    Static,
    Dynamic,
    Stream
};

enum class DrawMode {
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
    uint32_t size      {0};
    DataType dataType  {DataType::Float};
    bool normalized    {false};
    uint32_t offset    {0};
};

using VertexLayout = std::vector<VertexElement>;

// TODO: Improve constructor of VertexArray

class VertexArray {
public:
    VertexArray();
    VertexArray(const float* vertices, uint32_t verticesCount,
                VertexLayout& layout, DrawUsage vDrawUsage= DrawUsage::Static,
                const uint32_t* indices = nullptr, uint32_t indicesCount = 0, DrawUsage iDrawUsage = DrawUsage::Static);
    VertexArray(VertexArray&& other);
    VertexArray& operator=(VertexArray&& other);
    ~VertexArray();
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void Use();
    void Unbind();
    void Destroy();
    void Draw();

    void SetDrawMode(DrawMode drawMode);

    uint32_t GetID() const { return id; }
    uint32_t GetVerticesCount() const { return verticesCount; }
    uint32_t GetIndicesCount() const { return indicesCount; }

private:
    uint32_t id  {0};
    uint32_t vbo {0};
    uint32_t ibo {0};

    uint32_t verticesCount {0};
    uint32_t indicesCount {0};
    DrawMode drawMode {DrawMode::Triangles};
    
};

uint32_t CalculateVertexSizeAndOffsets(VertexLayout& layout);

#endif // __VERTEXARRAY_H__