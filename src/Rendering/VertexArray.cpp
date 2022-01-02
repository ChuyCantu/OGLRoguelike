#include "VertexArray.hpp"

#include "Core/Log.hpp"

#include <glad/glad.h>

uint32_t GetOpenGLDataType(DataType dataType) {
    switch (dataType) {
        case DataType::Bool   : return GL_BOOL;
        case DataType::Byte   : return GL_BYTE;
        case DataType::UByte  : return GL_UNSIGNED_BYTE;
        case DataType::Int    : return GL_INT;
        case DataType::Uint   : return GL_UNSIGNED_INT;
        case DataType::Float  : return GL_FLOAT;
        case DataType::Double : return GL_DOUBLE;
        default               : return GL_INVALID_ENUM;
    }
}

uint32_t GetDataTypeSize(DataType dataType) {
    switch (dataType) {
        case DataType::Bool   : return sizeof(bool);
        case DataType::Byte   : return sizeof(char);
        case DataType::UByte  : return sizeof(unsigned char);
        case DataType::Int    : return sizeof(int);
        case DataType::Uint   : return sizeof(unsigned int);
        case DataType::Float  : return sizeof(float);
        case DataType::Double : return sizeof(double);
        default               : return GL_INVALID_ENUM;
    }
}

uint32_t GetOpenGLDrawMode(DrawMode drawMode) {
    switch (drawMode) {
        case DrawMode::Points                 : return GL_POINTS;
        case DrawMode::LineStrip              : return GL_LINE_STRIP;
        case DrawMode::LineLoop               : return GL_LINE_LOOP;
        case DrawMode::Lines                  : return GL_LINES;
        case DrawMode::LineStripAdjacency     : return GL_LINE_STRIP_ADJACENCY;
        case DrawMode::LinesAdjacency         : return GL_LINES_ADJACENCY;
        case DrawMode::TriangleStrip          : return GL_TRIANGLE_STRIP;
        case DrawMode::TriangleFan            : return GL_TRIANGLE_FAN;
        case DrawMode::Triangles              : return GL_TRIANGLES;
        case DrawMode::TriangleStripAdjacency : return GL_TRIANGLE_STRIP_ADJACENCY;
        case DrawMode::TrianglesAdjacency     : return GL_TRIANGLES_ADJACENCY;
        case DrawMode::Patches                : return GL_PATCHES;
        default                               : return GL_INVALID_ENUM;
    }
}

VertexArray::VertexArray() {
}

VertexArray::VertexArray(const float* vertices, uint32_t verticesCount, VertexLayout& layout, DrawUsage vDrawUsage,
                         const uint32_t* indices, uint32_t indicesCount, DrawUsage iDrawUsage) 
    : verticesCount{verticesCount}, indicesCount{indicesCount} {
    uint32_t vertexSize {CalculateVertexSizeAndOffsets(layout)};

    GLenum drawUsage {GL_STATIC_DRAW};
    switch (vDrawUsage) {
        case DrawUsage::Static:  drawUsage = GL_STATIC_DRAW; break;
        case DrawUsage::Dynamic: drawUsage = GL_STATIC_DRAW; break;
        case DrawUsage::Stream:  drawUsage = GL_STREAM_DRAW; break;
    }

    glCreateBuffers(1, &vbo);
    int deb = vertexSize * verticesCount;
    glNamedBufferData(vbo, vertexSize * verticesCount, vertices, drawUsage);

    LOG_DEBUG("VBO [{}] created.", vbo);

    if (indices != nullptr && indicesCount != 0) {
        drawUsage = GL_STATIC_DRAW;
        switch (iDrawUsage) {
            case DrawUsage::Static:  drawUsage = GL_STATIC_DRAW; break;
            case DrawUsage::Dynamic: drawUsage = GL_STATIC_DRAW; break;
            case DrawUsage::Stream:  drawUsage = GL_STREAM_DRAW; break;
        }

        glCreateBuffers(1, &ibo);
        glNamedBufferData(ibo, sizeof(uint32_t) * indicesCount, indices, drawUsage);

        LOG_DEBUG("IBO [{}] created.", ibo);
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

    LOG_DEBUG("VAO [{}] created.", id);
}

VertexArray::VertexArray(VertexArray&& other)
    : id{other.id}, vbo{other.vbo}, ibo{other.ibo}, verticesCount{other.verticesCount}, indicesCount{other.indicesCount}, drawMode{other.drawMode} {
    other.id = 0;
    other.vbo = 0;
    other.ibo = 0;
}

VertexArray::~VertexArray() {
    Destroy();
}

VertexArray& VertexArray::operator=(VertexArray && other) {
    id = other.id;
    other.id = 0;
    vbo = other.vbo;
    other.vbo = 0;
    ibo = other.ibo;
    other.ibo = 0;
    verticesCount = other.verticesCount;
    indicesCount = other.indicesCount;
    drawMode = other.drawMode;
    return *this;
}

void VertexArray::Use() {
    glBindVertexArray(id);
}

void VertexArray::Unbind() {
    glBindVertexArray(0);
}

void VertexArray::Destroy() {
    if (id != 0) {
        LOG_DEBUG("VBO [{}] deleted.", vbo);
        glDeleteBuffers(1, &vbo);
        if (indicesCount != 0) {
            LOG_DEBUG("IBO [{}] deleted.", ibo);
            glDeleteBuffers(1, &ibo);
        }
        LOG_DEBUG("VAO [{}] deleted.", id);
        glDeleteVertexArrays(1, &id);
    }
}

void VertexArray::Draw() {
    if (indicesCount != 0) 
        glDrawElements(GetOpenGLDrawMode(drawMode), indicesCount, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GetOpenGLDrawMode(drawMode), 0, verticesCount);
}

void VertexArray::SetDrawMode(DrawMode drawMode) {
    this->drawMode = drawMode;
}

uint32_t CalculateVertexSizeAndOffsets(VertexLayout& layout) {
    uint32_t size {0};
    for (VertexElement& element : layout) {
        element.offset = size;
        size += element.size * GetDataTypeSize(element.dataType);
    }
    return size;
}