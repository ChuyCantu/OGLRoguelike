#ifndef __UNIFORMBUFFER_H__
#define __UNIFORMBUFFER_H__

#include "Buffer.hpp"

class UniformBuffer : public Buffer {
public: 
    UniformBuffer();
    UniformBuffer(uint32_t size, uint32_t bindingPointIndex, const void* data = nullptr, BufferUsage usage = BufferUsage::Dynamic);
    void Create(uint32_t size, uint32_t bindingPointIndex, const void* data = nullptr, BufferUsage usage = BufferUsage::Dynamic);

    void SetBufferTarget(BufferTarget) = delete;
};

#endif // __UNIFORMBUFFER_H__