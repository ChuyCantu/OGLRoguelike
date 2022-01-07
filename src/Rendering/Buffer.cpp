#include "Buffer.hpp"

#include "Core/Log.hpp"

#include <glad/glad.h>

uint32_t GetOpenGLDataType(DataType dataType) {
    switch (dataType) {
        case DataType::Bool     : return GL_BOOL;
        case DataType::Byte     : return GL_BYTE;
        case DataType::UByte    : return GL_UNSIGNED_BYTE;
        case DataType::Int      : return GL_INT;
        case DataType::UInt     : return GL_UNSIGNED_INT;
        case DataType::Float    : return GL_FLOAT;
        case DataType::Double   : return GL_DOUBLE;
        case DataType::UInt24_8 : return GL_UNSIGNED_INT_24_8;
        default                 : return GL_INVALID_ENUM;
    }
}

uint32_t GetDataTypeSize(DataType dataType) {
    switch (dataType) {
        case DataType::Bool     : return sizeof(bool);
        case DataType::Byte     : return sizeof(char);
        case DataType::UByte    : return sizeof(unsigned char);
        case DataType::Int      : return sizeof(int);
        case DataType::UInt     : return sizeof(unsigned int);
        case DataType::Float    : return sizeof(float);
        case DataType::Double   : return sizeof(double);
        case DataType::UInt24_8 : return sizeof(unsigned int);
        default                 : return GL_INVALID_ENUM;
    }
}

uint32_t GetOpenGLBufferUsage(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::Static  : return GL_STATIC_DRAW;
        case BufferUsage::Dynamic : return GL_STATIC_DRAW;
        case BufferUsage::Stream  : return GL_STREAM_DRAW;
        default                   : return GL_INVALID_ENUM;
    }
}

uint32_t GetOpenGLBufferTarget(BufferTarget target) {
    switch (target) {
        case BufferTarget::ArrayBuffer             : return GL_ARRAY_BUFFER;
        case BufferTarget::AtomicCounterBuffer     : return GL_ATOMIC_COUNTER_BUFFER;
        case BufferTarget::CopyReadBuffer          : return GL_COPY_READ_BUFFER;
        case BufferTarget::CopyWriteBuffer         : return GL_COPY_WRITE_BUFFER;
        case BufferTarget::DispatchIndirectBuffer  : return GL_DISPATCH_INDIRECT_BUFFER;
        case BufferTarget::DrawIndirectBuffer      : return GL_DRAW_INDIRECT_BUFFER;
        case BufferTarget::ElementArrayBuffer      : return GL_ELEMENT_ARRAY_BUFFER;
        case BufferTarget::PixelPackBuffer         : return GL_PIXEL_PACK_BUFFER;
        case BufferTarget::PixelUnpackBuffer       : return GL_PIXEL_UNPACK_BUFFER;
        case BufferTarget::QueryBuffer             : return GL_QUERY_BUFFER;
        case BufferTarget::ShaderStorageBuffer     : return GL_SHADER_STORAGE_BUFFER;
        case BufferTarget::TextureBuffer           : return GL_TEXTURE_BUFFER;
        case BufferTarget::TransformFeedbackBuffer : return GL_TRANSFORM_FEEDBACK_BUFFER;
        case BufferTarget::UniformBuffer           : return GL_UNIFORM_BUFFER;
        case BufferTarget::Null                    : return GL_INVALID_ENUM;
        default                                    : return GL_INVALID_ENUM;
    }
}

Buffer::Buffer() { }

Buffer::Buffer(uint32_t size, const void* data, BufferUsage usage, BufferTarget target) {
    Create(size, data, usage, target);
}

Buffer::Buffer(Buffer&& other) 
    : id{other.id}, size{other.size}, target{other.target} {
    other.id = 0;
}

Buffer& Buffer::operator=(Buffer&& other) {
    Destroy();
    id = other.id;
    other.id = 0;
    size = other.size;
    target = other.target;
    return *this;
}

Buffer::~Buffer() {
    Destroy();
}

void Buffer::Create(uint32_t size, const void* data, BufferUsage usage, BufferTarget target) {
    Destroy();
    glCreateBuffers(1, &id);
    glNamedBufferData(id, size, data, GetOpenGLBufferUsage(usage));
    this->size = size;
    this->target = target;

    LOG_DEBUG("Buffer [{}] created.", id);
}

void Buffer::SetData(uint32_t offset, uint32_t size, const void* data) {
    glNamedBufferSubData(id, offset, size, data);
}

Buffer& Buffer::Bind() {
    glBindBuffer(GetOpenGLBufferTarget(target), id); 
    return *this;
}

void Buffer::Unbind() const{
    glBindBuffer(GetOpenGLBufferTarget(target), 0);
}

void Buffer::Destroy() {
    if (id != 0) {
        LOG_DEBUG("Buffer [{}] deleted.", id);
        glDeleteBuffers(1, &id);
        id = 0;
    }
}

void Buffer::SetBufferTarget(BufferTarget target) {
    this->target = target;
}

void CopyBufferSubData(Buffer& src, Buffer& dest, uint32_t srcOffset, uint32_t destOffset, size_t size) {
    glCopyNamedBufferSubData(src.id, dest.id, srcOffset, destOffset, size);
}

