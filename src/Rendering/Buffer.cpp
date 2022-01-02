#include "Buffer.hpp"

#include <glad/glad.h>

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

Buffer::Buffer(uint32_t size, BufferUsage usage, const void* data, BufferTarget target) {
    Create(size, usage, data, target);
}

Buffer::Buffer(Buffer&& other) 
    : id{other.id}, target{other.target} {
    other.id = 0;
}

Buffer& Buffer::operator=(Buffer&& other) {
    id = other.id;
    other.id = 0;
    target = other.target;
    return *this;
}

Buffer::~Buffer() {
    Destroy();
}

void Buffer::Create(uint32_t size, BufferUsage usage, const void* data, BufferTarget target) {
    Destroy();
    glCreateBuffers(1, &id);
    glNamedBufferData(id, size, data, GetOpenGLBufferUsage(usage));
    this->target = target;
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
        glDeleteBuffers(1, &id);
    }
}

void Buffer::SetBufferTarget(BufferTarget target) {
    this->target = target;
}

void CopyBufferSubData(Buffer& src, Buffer& dest, uint32_t srcOffset, uint32_t destOffset, size_t size) {
    glCopyNamedBufferSubData(src.id, dest.id, srcOffset, destOffset, size);
}

