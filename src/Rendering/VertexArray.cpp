#include "VertexArray.hpp"

#include "Core/Log.hpp"

#include <glad/glad.h>

uint32_t GetOpenGLDataType(DataType dataType) {
    switch (dataType) {
        case DataType::Bool:   return GL_BOOL;
        case DataType::Byte:   return GL_BYTE;
        case DataType::UByte:  return GL_UNSIGNED_BYTE;
        case DataType::Int:    return GL_INT;
        case DataType::Uint:   return GL_UNSIGNED_INT;
        case DataType::Float:  return GL_FLOAT;
        case DataType::Double: return GL_DOUBLE;
        default: return GL_ID_UNKNOWN;
    }
}

uint32_t GetDataTypeSize(DataType dataType) {
    switch (dataType) {
        case DataType::Bool:   return sizeof(bool);
        case DataType::Byte:   return sizeof(char);
        case DataType::UByte:  return sizeof(unsigned char);
        case DataType::Int:    return sizeof(int);
        case DataType::Uint:   return sizeof(unsigned int);
        case DataType::Float:  return sizeof(float);
        case DataType::Double: return sizeof(double);
        default: return GL_ID_UNKNOWN;
    }
}

VertexArray::VertexArray() {
    
}

VertexArray::VertexArray(const float* vertices, uint32_t verticesCount, VertexLayout& layout, DrawMode vDrawMode,
                         const uint32_t* indices, uint32_t indicesCount, DrawMode iDrawMode) 
    : verticesCount{verticesCount}, indicesCount{indicesCount} {
    uint32_t vertexSize {CalculateVertexSizeAndOffsets(layout)};

    GLenum drawUsage {GL_STATIC_DRAW};
    switch (vDrawMode) {
        case DrawMode::Static:  drawUsage = GL_STATIC_DRAW;
        case DrawMode::Dynamic: drawUsage = GL_STATIC_DRAW;
        case DrawMode::Stream:  drawUsage = GL_STREAM_DRAW;
    }

    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, vertexSize * verticesCount, vertices, drawUsage);

    if (indices != nullptr && indicesCount != 0) {
        drawUsage = GL_STATIC_DRAW;
        switch (iDrawMode) {
            case DrawMode::Static:  drawUsage = GL_STATIC_DRAW;
            case DrawMode::Dynamic: drawUsage = GL_STATIC_DRAW;
            case DrawMode::Stream:  drawUsage = GL_STREAM_DRAW;
        }

        glCreateBuffers(1, &ibo);
        glNamedBufferData(ibo, sizeof(uint32_t) * indicesCount, indices, drawUsage);
    }

    glCreateVertexArrays(1, &id);
    glVertexArrayVertexBuffer(id, 0, vbo, 0, vertexSize);
    if (indices != nullptr && indicesCount != 0) 
        glVertexArrayElementBuffer(id, ibo);


    for (size_t i {0}; i < layout.size(); ++i) {
        glEnableVertexArrayAttrib(id, i);
        glVertexArrayAttribFormat(id, i, layout[i].size, GetOpenGLDataType(layout[i].dataType), 
                                  layout[i].normalized, layout[i].offset);
        glVertexArrayAttribBinding(id, i, 0);
    }
}

VertexArray::~VertexArray() {
    
}

void VertexArray::Use() {
    glBindVertexArray(id);
}

void VertexArray::Unbind() {
    glBindVertexArray(0);
}

void VertexArray::Destroy() {
    glDeleteBuffers(1, &vbo);
    if (indicesCount != 0)
        glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &id);
}

void VertexArray::Draw() {
    if (indicesCount != 0) 
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, verticesCount);
}

uint32_t CalculateVertexSizeAndOffsets(VertexLayout& layout) {
    uint32_t size {0};
    for (VertexElement& element : layout) {
        element.offset = size;
        size += element.size * GetDataTypeSize(element.dataType);
    }
    return size;
}