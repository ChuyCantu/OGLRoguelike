#pragma once

#include "Buffer.hpp"

class UniformBuffer : public Buffer {
public: 
    UniformBuffer();
    UniformBuffer(uint32_t size, uint32_t bindingPointIndex, const void* data = nullptr, BufferUsage usage = BufferUsage::Dynamic);
    void Create(uint32_t size, uint32_t bindingPointIndex, const void* data = nullptr, BufferUsage usage = BufferUsage::Dynamic);

    void SetBufferTarget(BufferTarget) = delete;
};