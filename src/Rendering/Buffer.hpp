#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>

enum class BufferUsage : uint32_t {
    Static,
    Dynamic,
    Stream
};

uint32_t GetOpenGLBufferUsage(BufferUsage usage);

enum class BufferTarget : uint32_t {
    ArrayBuffer,
    AtomicCounterBuffer,
    CopyReadBuffer,
    CopyWriteBuffer,
    DispatchIndirectBuffer,
    DrawIndirectBuffer,
    ElementArrayBuffer,
    PixelPackBuffer,
    PixelUnpackBuffer,
    QueryBuffer,
    ShaderStorageBuffer,
    TextureBuffer,
    TransformFeedbackBuffer,
    UniformBuffer,
    Null
};

uint32_t GetOpenGLBufferTarget(BufferTarget target);

class Buffer {
public:
    Buffer();
    // You can leave buffer target with the default value if you know that your won't explicitly need to bind the buffer
    Buffer(uint32_t size, const void* data = nullptr, BufferUsage usage = BufferUsage::Static, BufferTarget target = BufferTarget::Null);
    Buffer(Buffer&& other);
    Buffer& operator=(Buffer&& other);
    virtual ~Buffer();
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    void Create(uint32_t size, const void* data = nullptr, BufferUsage usage = BufferUsage::Static, BufferTarget target = BufferTarget::Null);
    void SetData(uint32_t offset, uint32_t size, const void* data);
    void Destroy();
    Buffer& Bind();
    void Unbind() const;
    void SetBufferTarget(BufferTarget target);
    bool IsNull() const { return id == 0; }
    uint32_t GetID() { return id; }

    friend void CopyBufferSubData(Buffer& src, Buffer& dest, uint32_t srcOffset, uint32_t destOffset, size_t size);

protected:
    uint32_t id {0};
    BufferTarget target;
};

#endif // __BUFFER_H__