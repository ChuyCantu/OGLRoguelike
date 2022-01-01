#ifndef __VERTEXARRAY_H__
#define __VERTEXARRAY_H__

#include <stdint.h>
#include <vector>

enum class DataType {
    Bool, Byte, UByte, Int, Uint, Float, Double
};

uint32_t GetOpenGLDataType(DataType dataType);
uint32_t GetDataTypeSize(DataType dataType);

enum class DrawMode {
    Static, Dynamic, Stream
};

struct VertexElement {
    uint32_t size      {0};
    DataType dataType  {DataType::Float};
    bool normalized    {false};
    uint32_t offset    {0};
};

using VertexLayout = std::vector<VertexElement>;

class VertexArray {
public:
    VertexArray();
    VertexArray(const float* vertices, uint32_t verticesCount,
                VertexLayout& layout, DrawMode vDrawMode = DrawMode::Static,
                const uint32_t* indices = nullptr, uint32_t indicesCount = 0, DrawMode iDrawMode = DrawMode::Static);
    ~VertexArray();

    void Use();
    void Unbind();
    void Destroy();
    void Draw();

    uint32_t GetID() const { return id; }
    uint32_t GetVerticesCount() const { return verticesCount; }
    uint32_t GetIndicesCount() const { return indicesCount; }

private:
    uint32_t id  {0};
    uint32_t vbo {0};
    uint32_t ibo {0};

    uint32_t verticesCount {0};
    uint32_t indicesCount {0};
    
};

uint32_t CalculateVertexSizeAndOffsets(VertexLayout& layout);

#endif // __VERTEXARRAY_H__