#include "UniformBuffer.hpp"

#include <glad/glad.h>

UniformBuffer::UniformBuffer() { }

UniformBuffer::UniformBuffer(uint32_t size, uint32_t bindingPointIndex, const void* data, BufferUsage usage) {
    UniformBuffer::Create(size, bindingPointIndex, data, usage);
}

void UniformBuffer::Create(uint32_t size, uint32_t bindingPointIndex, const void* data, BufferUsage usage) {
    Buffer::Create(size, data, usage, BufferTarget::UniformBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointIndex, id);
}
