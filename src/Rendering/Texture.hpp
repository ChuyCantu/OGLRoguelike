#pragma once

#include "Buffer.hpp"

#include <glad/glad.h>
#include <string>

enum class TextureTarget : uint32_t {
    Texture_1D,
    Texture_1DArray,
    Texture_2D,
    Texture_2DArray,
    Texture_2DMultisample,
    Texture_2DMultisampleArray,
    Texture_3D,
    TextureCubeMap,
    TextureCubeMapArray,
    TextureRectangle
};

uint32_t ToOpenGL(TextureTarget target);

enum class TextureParamName : uint32_t {
    DepthStencilTextureMode,
    TextureBaseLevel,
    TextureBorderColor,
    TextureCompareFunc,
    TextureCompareMode,
    TextureLodBias,
    TextureMinFilter,
    TextureMagFilter,
    TextureMinLod,
    TextureMaxLod,
    TextureMaxLevel,
    TextureSwizzleR,
    TextureSwizzleG,
    TextureSwizzleB,
    TextureSwizzleA,
    TextureWrapS,
    TextureWrapT,
    TextureWrapR

    //+ For the vector commands (glTexParameter*v):
    // GL_TEXTURE_BORDER_COLOR,
    // GL_TEXTURE_SWIZZLE_RGBA
};

uint32_t ToOpenGL(TextureParamName pname);

// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
enum class TextureParameter : uint32_t {
    DepthComponent,
    StencilIndex,
    Lequal,
    Gequal,
    Less,
    Greater,
    Equal,
    Notequal,
    Always,
    Never,
    CompareRefToTexture,
    None,
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear,
    Red,
    Green,
    Blue,
    Alpha,
    Zero,
    One,
    ClampToEdge,
    ClampToBorder,
    MirroredRepeat,
    Repeat,
    MirrorClampToEdge
};

uint32_t ToOpenGL(TextureParameter param);

enum class TextureFormat : uint32_t {
    RED,
    RG,
    RGB,
    BGR,
    RGBA,
    BGRA,
    DepthComponent,
    StencilIndex,
    Depth_Stencil,

    //+ Internal Formats:
    R8,
    R8SNORM,
    R16,
    R16SNORM,
    RG8,
    RG8SNORM,
    RG16,
    RG16SNORM,
    R3G3B2,
    RGB4,
    RGB5,
    RGB8,
    RGB8SNORM,
    RGB10,
    RGB12,
    RGB16SNORM,
    RGBA2,
    RGBA4,
    RGB5A1,
    RGBA8,
    RGBA8SNORM,
    RGB10A2,
    RGB10A2UI,
    RGBA12,
    RGBA16,
    SRGB8,
    SRGB8ALPHA8,
    R16F,
    RG16F,
    RGB16F,
    RGBA16F,
    R32F,
    RG32F,
    RGB32F,
    RGBA32F,
    R11FG11FB10F,
    RGB9E5,
    R8I,
    R8UI,
    R16I,
    R16UI,
    R32I,
    R32UI,
    RG8I,
    RG8UI,
    RG16I,
    RG16UI,
    RG32I,
    RG32UI,
    RGB8I,
    RGB8UI,
    RGB16I,
    RGB16UI,
    RGB32I,
    RGB32UI,
    RGBA8I,
    RGBA8UI,
    RGBA16I,
    RGBA16UI,
    RGBA32I,
    RGBA32UI,
    Depth24_Stencil8
};

uint32_t ToOpenGL(TextureFormat format);

class Texture {
   public:
    Texture();
    Texture(const std::string& fileName, bool flipYAxis = false);
    Texture(Texture&& other);
    Texture& operator=(Texture&& other);
    ~Texture();
    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;

    bool Load(const std::string& fileName, bool flipYAxis = false);
    void Generate(uint32_t width, uint32_t height, const void* pixels, TextureFormat internalFormat, TextureFormat imageFormat, DataType type = DataType::UByte);
    void SubImage(uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height, const void* pixels, DataType type = DataType::UByte);
    void Unload();
    void Use(int index = 0) const;
    void Unbind() const;
    bool IsNull() const { return id == 0; }

    Texture& SetWrapS(TextureParameter param);
    Texture& SetWrapT(TextureParameter param);
    Texture& SetMinFilter(TextureParameter param);
    Texture& SetMagFilter(TextureParameter param);
    Texture& GenerateMipmap();
    Texture& SetMaxAnisotropy(float anisotropy);

    uint32_t GetID() const { return id; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    const std::string& GetPath() const { return path; }
    TextureFormat GetInternalFormat() const { return internalFormat; }
    TextureFormat GetImageFormat() const { return imageFormat; }
    TextureParameter GetWrapS() const { return wrapS; }
    TextureParameter GetWrapT() const { return wrapT; }
    TextureParameter GetMinFilter() const { return minFilter; }
    TextureParameter GetMagFiler() const { return magFiler; }
    bool HasMipmap() const { return hasMipmap; }

    static float GetMaxAnisotropicLevel();

private:
    uint32_t id{0};
    int width{0};
    int height{0};

    std::string path;

    TextureFormat internalFormat;  // Format of texture object
    TextureFormat imageFormat;

    TextureParameter wrapS;
    TextureParameter wrapT;
    TextureParameter minFilter;
    TextureParameter magFiler;

    bool hasMipmap;
};