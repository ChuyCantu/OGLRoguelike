#include "VertexArray.hpp"

#include "Core/Log.hpp"

#include <glad/glad.h>

uint32_t ToOpenGL(DrawMode drawMode) {
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

VertexArray::VertexArray(const void* vertices, uint32_t verticesCount, VertexLayout& layout, BufferUsage vDrawUsage,
                         const uint32_t* indices, uint32_t indicesCount, BufferUsage iDrawUsage) 
    : verticesCount{verticesCount}, indicesCount{indicesCount} {
    uint32_t vertexSize {CalculateVertexSizeAndOffsets(layout)};

    vbo.Create(vertexSize * verticesCount, vertices, vDrawUsage, BufferTarget::ArrayBuffer);

    LOG_DEBUG("VBO [{}] created.", vbo.GetID());

    if (indices != nullptr && indicesCount != 0) {
        ibo.Create(sizeof(uint32_t) * indicesCount, indices, iDrawUsage, BufferTarget::ElementArrayBuffer);

        LOG_DEBUG("IBO [{}] created.", ibo.GetID());
    }

    glCreateVertexArrays(1, &id);
    glVertexArrayVertexBuffer(id, 0, vbo.GetID(), 0, vertexSize);
    if (indices != nullptr && indicesCount != 0) 
        glVertexArrayElementBuffer(id, ibo.GetID());

    // Note on AttribFormat and AttribIFormat: https://community.khronos.org/t/vertex-shader-integer-input-broken/72878
    for (size_t i {0}; i < layout.size(); ++i) {
        glEnableVertexArrayAttrib(id, i);
        if (!layout[i].useIntegerFormat)
            glVertexArrayAttribFormat(id, i, layout[i].size, ToOpenGL(layout[i].dataType),
                                      layout[i].normalized, layout[i].offset);
        else
            glVertexArrayAttribIFormat(id, i, layout[i].size, ToOpenGL(layout[i].dataType), layout[i].offset);
        glVertexArrayAttribBinding(id, i, 0);
    }

    LOG_DEBUG("VAO [{}] created.", id);
}

VertexArray::VertexArray(VertexArray&& other)
    : id{other.id}, vbo{std::move(other.vbo)}, ibo{std::move(other.ibo)}, verticesCount{other.verticesCount}, indicesCount{other.indicesCount}, drawMode{other.drawMode} {
    other.id = 0;
}

VertexArray::~VertexArray() {
    Destroy();
}

VertexArray& VertexArray::operator=(VertexArray && other) {
    Destroy();
    id = other.id;
    other.id = 0;
    vbo = std::move(other.vbo);
    ibo = std::move(other.ibo);
    verticesCount = other.verticesCount;
    indicesCount = other.indicesCount;
    drawMode = other.drawMode;
    return *this;
}

void VertexArray::Use() const {
    glBindVertexArray(id);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::Destroy() {
    if (id != 0) {
        LOG_DEBUG("VBO [{}] deleted.", vbo.GetID());
        vbo.Destroy();
        if (indicesCount != 0) {
            LOG_DEBUG("IBO [{}] deleted.", ibo.GetID());
            ibo.Destroy();
        }
        LOG_DEBUG("VAO [{}] deleted.", id);
        glDeleteVertexArrays(1, &id);
        id = 0;
    }
}

void VertexArray::Draw() const {
    if (indicesCount != 0)
        glDrawElements(ToOpenGL(drawMode), indicesCount, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(ToOpenGL(drawMode), 0, verticesCount);
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