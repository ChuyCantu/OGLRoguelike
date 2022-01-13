#include "Framebuffer.hpp"

#include "Buffer.hpp"
#include "Core/Log.hpp"

#include <glad/glad.h>

Framebuffer::Framebuffer() { }

Framebuffer::Framebuffer(Framebuffer&& other)
    : id{other.id}, renderBuffers{std::move(other.renderBuffers)} {
    other.id = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) {
    id = other.id;
    other.id = 0;
    renderBuffers = std::move(other.renderBuffers);
    return *this;
}

Framebuffer::~Framebuffer() {
    Destroy();
}

void Framebuffer::Create() {
    Destroy();

    glCreateFramebuffers(1, &id);
    LOG_DEBUG("Buffer [{}] created.", id);
}

void Framebuffer::Destroy() {
    if (id != 0) {
        for (uint32_t rbo : renderBuffers) {
            LOG_DEBUG("RenderBuffer [{}] deleted.", id);
            glDeleteRenderbuffers(1, &rbo);
        }

        LOG_DEBUG("Framebuffer [{}] deleted.", id);
        glDeleteFramebuffers(1, &id);
    }
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::AddColorTexture(int colorIndex, const Texture* texture, int mipmapLevel) {
    glNamedFramebufferTexture(id, GL_COLOR_ATTACHMENT0 + colorIndex, texture->GetID(), mipmapLevel);
}

void Framebuffer::AddDepthTexture(const Texture* texture, int mipmapLevel) {
    glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, texture->GetID(), mipmapLevel);
}

void Framebuffer::AddStencilTexture(const Texture* texture, int mipmapLevel) {
    glNamedFramebufferTexture(id, GL_STENCIL_ATTACHMENT, texture->GetID(), mipmapLevel);
}

void Framebuffer::AddDepthStencilTexture(const Texture* texture, int mipmapLevel) {
    glNamedFramebufferTexture(id, GL_DEPTH_STENCIL_ATTACHMENT, texture->GetID(), mipmapLevel);
}

void Framebuffer::AddColorRenderbuffer(int colorIndex, int width, int height, TextureFormat internalFormat, int samples) {
    uint32_t rbo;
    glCreateRenderbuffers(1, &rbo);
    if (samples == 0)
        glNamedRenderbufferStorage(rbo, ToOpenGL(internalFormat), width, height);
    else
        glNamedRenderbufferStorageMultisample(rbo, samples, ToOpenGL(internalFormat), width, height);

    glNamedFramebufferRenderbuffer(id, GL_COLOR_ATTACHMENT0 + colorIndex, GL_RENDERBUFFER, rbo);

    renderBuffers.push_back(rbo);
}

void Framebuffer::AddDepthRenderbuffer(int width, int height, TextureFormat internalFormat, int samples) {
    uint32_t rbo;
    glCreateRenderbuffers(1, &rbo);
    if (samples == 0)
        glNamedRenderbufferStorage(rbo, ToOpenGL(internalFormat), width, height);
    else
        glNamedRenderbufferStorageMultisample(rbo, samples, ToOpenGL(internalFormat), width, height);

    glNamedFramebufferRenderbuffer(id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    renderBuffers.push_back(rbo);
}

void Framebuffer::AddStencilRenderbuffer(int width, int height, TextureFormat internalFormat, int samples) {
    uint32_t rbo;
    glCreateRenderbuffers(1, &rbo);
    if (samples == 0)
        glNamedRenderbufferStorage(rbo, ToOpenGL(internalFormat), width, height);
    else
        glNamedRenderbufferStorageMultisample(rbo, samples, ToOpenGL(internalFormat), width, height);

    glNamedFramebufferRenderbuffer(id, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    renderBuffers.push_back(rbo);
}

void Framebuffer::AddDepthStencilRenderbuffer(int width, int height, TextureFormat internalFormat, int samples) {
    uint32_t rbo;
    glCreateRenderbuffers(1, &rbo);
    if (samples == 0)
        glNamedRenderbufferStorage(rbo, ToOpenGL(internalFormat), width, height);
    else
        glNamedRenderbufferStorageMultisample(rbo, samples, ToOpenGL(internalFormat), width, height);

    glNamedFramebufferRenderbuffer(id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    renderBuffers.push_back(rbo);
}

bool Framebuffer::CheckStatus() const {
    bool status {glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE};
    LOGIF_DEBUG(!status, "Failed to create Framebuffer [{}]", id);
    return status;
}

void Framebuffer::ClearColor(int* rgba, int drawBuffer) {
    glClearNamedFramebufferiv(id, GL_COLOR, drawBuffer, rgba);
}

void Framebuffer::ClearColor(float* rgba, int drawBuffer) {
    glClearNamedFramebufferfv(id, GL_COLOR, drawBuffer, rgba);
}

void Framebuffer::ClearColor(uint32_t* rgba, int drawBuffer) {
    glClearNamedFramebufferuiv(id, GL_COLOR, drawBuffer, rgba);
}

void Framebuffer::ClearDepth(float* value) {
    glClearNamedFramebufferfv(id, GL_DEPTH, 0, value);
}

void Framebuffer::ClearStencil(int* value) {
    glClearNamedFramebufferiv(id, GL_STENCIL, 0, value);
}

void Framebuffer::ClearDepthStencil(float depth, int stencil) {
    glClearNamedFramebufferfi(id, GL_DEPTH_STENCIL, 0, depth, stencil); 
}


void BlitFramebuffer(Framebuffer src, Framebuffer& dest,
                     int srcX0, int srcY0, int srcX1, int srcY1,
                     int dstX0, int dstY0, int dstX1, int dstY1,
                     GLbitfield mask, GLenum filter) {
    glBlitNamedFramebuffer(src.GetID(), dest.GetID(), 
                           srcX0, srcY0, srcX1, srcY1,
                           dstX0, dstY0, dstX1, dstY1,
                           mask, filter);
}