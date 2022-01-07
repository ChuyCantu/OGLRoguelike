#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "Texture.hpp"

#include <stdint.h>
#include <vector>

// enum class FramebufferAttachment {
//     Color, Depth, Stencil, Depth_Stencil
// };

class Framebuffer {
public:
    Framebuffer();
    Framebuffer(Framebuffer&& other);
    Framebuffer& operator=(Framebuffer&& other);
    ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    void Create();
    void Destroy();
    Framebuffer& Bind();
    void Unbind() const;

    void AddColorTexture(int colorIndex, const class Texture* texture, int mipmapLevel);
    void AddDepthTexture(const Texture* texture, int mipmapLevel);
    void AddStencilTexture(const Texture* texture, int mipmapLevel);
    void AddDepthStencilTexture(const Texture* texture, int mipmapLevel);
    void AddColorRenderbuffer(int colorIndex, int width, int height, TextureFormat internalFormat, int samples = 0);
    void AddDepthRenderbuffer(int width, int height, TextureFormat internalFormat, int samples);
    void AddStencilRenderbuffer(int width, int height, TextureFormat internalFormat, int samples);
    void AddDepthStencilRenderbuffer(int width, int height, TextureFormat internalFormat, int samples);

    bool CheckStatus();

    void ClearColor(int* rgba, int drawBuffer = 0);
    void ClearColor(float* rgba, int drawBuffer = 0);
    void ClearColor(uint32_t* rgba, int drawBuffer = 0);
    void ClearDepth(float* value);
    void ClearStencil(int* value);
    void ClearDepthStencil(float depth, int stencil);

    uint32_t GetID() const { return id; }
    bool IsDefaultFrameBuffer() const { return id == 0; }

    friend void BlitFramebuffer(Framebuffer src, Framebuffer& dest,
                                int srcX0, int srcY0, int srcX1, int srcY1,
                                int dstX0, int dstY0, int dstX1, int dstY1,
                                GLbitfield mask, GLenum filter);

private:
    uint32_t id {0};
    std::vector<uint32_t> renderBuffers;
};

#endif // __FRAMEBUFFER_H__