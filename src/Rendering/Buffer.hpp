#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>

enum class DataType : uint32_t {
    Bool,
    Byte,
    UByte,
    Short,
    UShort,
    Int,
    UInt,
    Float,
    Double,
    UInt24_8
};

uint32_t ToOpenGL(DataType dataType);
uint32_t GetDataTypeSize(DataType dataType);

enum class BufferUsage : uint32_t {
    Static,
    Dynamic,
    Stream
};

uint32_t ToOpenGL(BufferUsage usage);

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

uint32_t ToOpenGL(BufferTarget target);

enum class BufferAccess : uint32_t {
    ReadOnly,
    WriteOnly,
    ReadWrite
};

uint32_t ToOpenGL(BufferAccess access);

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
    void* Map(BufferAccess access);
    void Unmap();
    void Destroy();
    void Bind() const;
    void Unbind() const;
    void SetBufferTarget(BufferTarget target);
    bool IsNull() const { return id == 0; }
    uint32_t GetSize() const { return size; }
    uint32_t GetID() const { return id; }

    friend void CopyBufferSubData(Buffer& src, Buffer& dest, uint32_t srcOffset, uint32_t destOffset, size_t size);

protected:
    uint32_t id   {0};
    uint32_t size {0};
    BufferTarget target;
};

#endif // __BUFFER_H__