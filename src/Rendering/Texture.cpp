#include "Texture.hpp"

#include "Core/Log.hpp"

#include <stb_image.h>

#define OGL_DSA

uint32_t GetOpenGLTextureTarget(TextureTarget target)  {
    switch (target)
    { 
        case TextureTarget::Texture_1D                 : return GL_TEXTURE_1D;
        case TextureTarget::Texture_1DArray            : return GL_TEXTURE_1D_ARRAY;
        case TextureTarget::Texture_2D                 : return GL_TEXTURE_2D;
        case TextureTarget::Texture_2DArray            : return GL_TEXTURE_2D_ARRAY;
        case TextureTarget::Texture_2DMultisample      : return GL_TEXTURE_2D_MULTISAMPLE;
        case TextureTarget::Texture_2DMultisampleArray : return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        case TextureTarget::Texture_3D                 : return GL_TEXTURE_3D;
        case TextureTarget::TextureCubeMap             : return GL_TEXTURE_CUBE_MAP;
        case TextureTarget::TextureCubeMapArray        : return GL_TEXTURE_CUBE_MAP_ARRAY;
        case TextureTarget::TextureRectangle           : return GL_TEXTURE_RECTANGLE;
        default                                        : return GL_INVALID_ENUM;
    }
}

uint32_t GetOpenGLTextureParamName(TextureParamName pname) {
    switch (pname) {
        case TextureParamName::DepthStencilTextureMode : return GL_DEPTH_STENCIL_TEXTURE_MODE;
        case TextureParamName::TextureBaseLevel        : return GL_TEXTURE_BASE_LEVEL;
        case TextureParamName::TextureBorderColor      : return GL_TEXTURE_BORDER_COLOR;
        case TextureParamName::TextureCompareFunc      : return GL_TEXTURE_COMPARE_FUNC;
        case TextureParamName::TextureCompareMode      : return GL_TEXTURE_COMPARE_MODE;
        case TextureParamName::TextureLodBias          : return GL_TEXTURE_LOD_BIAS;
        case TextureParamName::TextureMinFilter        : return GL_TEXTURE_MIN_FILTER;
        case TextureParamName::TextureMagFilter        : return GL_TEXTURE_MAG_FILTER;
        case TextureParamName::TextureMinLod           : return GL_TEXTURE_MIN_LOD;
        case TextureParamName::TextureMaxLod           : return GL_TEXTURE_MAX_LOD;
        case TextureParamName::TextureMaxLevel         : return GL_TEXTURE_MAX_LEVEL;
        case TextureParamName::TextureSwizzleR         : return GL_TEXTURE_SWIZZLE_R;
        case TextureParamName::TextureSwizzleG         : return GL_TEXTURE_SWIZZLE_G;
        case TextureParamName::TextureSwizzleB         : return GL_TEXTURE_SWIZZLE_B;
        case TextureParamName::TextureSwizzleA         : return GL_TEXTURE_SWIZZLE_A;
        case TextureParamName::TextureWrapS            : return GL_TEXTURE_WRAP_S;
        case TextureParamName::TextureWrapT            : return GL_TEXTURE_WRAP_T;
        case TextureParamName::TextureWrapR            : return GL_TEXTURE_WRAP_R;
        default                                        : return GL_INVALID_ENUM;
    }
}

uint32_t GetOpenGLTextureParameter(TextureParameter param) {
    switch (param) {
        case TextureParameter::DepthComponent       : return GL_DEPTH_COMPONENT;
        case TextureParameter::StencilIndex         : return GL_STENCIL_INDEX;
        case TextureParameter::Lequal               : return GL_LEQUAL;
        case TextureParameter::Gequal               : return GL_GEQUAL;
        case TextureParameter::Less                 : return GL_LESS;
        case TextureParameter::Greater              : return GL_GREATER;
        case TextureParameter::Equal                : return GL_EQUAL;
        case TextureParameter::Notequal             : return GL_NOTEQUAL;
        case TextureParameter::Always               : return GL_ALWAYS;
        case TextureParameter::Never                : return GL_NEVER;
        case TextureParameter::CompareRefToTexture  : return GL_COMPARE_REF_TO_TEXTURE;
        case TextureParameter::None                 : return GL_NONE;
        case TextureParameter::Nearest              : return GL_NEAREST;
        case TextureParameter::Linear               : return GL_LINEAR;
        case TextureParameter::NearestMipmapNearest : return GL_NEAREST_MIPMAP_NEAREST;
        case TextureParameter::LinearMipmapNearest  : return GL_LINEAR_MIPMAP_NEAREST;
        case TextureParameter::NearestMipmapLinear  : return GL_NEAREST_MIPMAP_LINEAR;
        case TextureParameter::LinearMipmapLinear   : return GL_LINEAR_MIPMAP_LINEAR;
        case TextureParameter::Red                  : return GL_RED;
        case TextureParameter::Green                : return GL_GREEN;
        case TextureParameter::Blue                 : return GL_BLUE;
        case TextureParameter::Alpha                : return GL_ALPHA;
        case TextureParameter::Zero                 : return GL_ZERO;
        case TextureParameter::One                  : return GL_ONE;
        case TextureParameter::ClampToEdge          : return GL_CLAMP_TO_EDGE;
        case TextureParameter::ClampToBorder        : return GL_CLAMP_TO_BORDER;
        case TextureParameter::MirroredRepeat       : return GL_MIRRORED_REPEAT;
        case TextureParameter::Repeat               : return GL_REPEAT;
        case TextureParameter::MirrorClampToEdge    : return GL_MIRROR_CLAMP_TO_EDGE;
        default                                     : return GL_INVALID_ENUM;
    }
}

uint32_t GetOpenGLTextureFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::RED              : return GL_RED;
        case TextureFormat::RG               : return GL_RG;
        case TextureFormat::RGB              : return GL_RGB;
        case TextureFormat::BGR              : return GL_BGR;
        case TextureFormat::RGBA             : return GL_RGBA;
        case TextureFormat::BGRA             : return GL_BGRA;
        case TextureFormat::DepthComponent   : return GL_DEPTH_COMPONENT;
        case TextureFormat::StencilIndex     : return GL_STENCIL_INDEX;
        case TextureFormat::Depth_Stencil    : return GL_DEPTH_STENCIL;
          
        case TextureFormat::R8               : return GL_R8;
        case TextureFormat::R8SNORM          : return GL_R8_SNORM;
        case TextureFormat::R16              : return GL_R16;
        case TextureFormat::R16SNORM         : return GL_R16_SNORM;
        case TextureFormat::RG8              : return GL_RG8;
        case TextureFormat::RG8SNORM         : return GL_RG8_SNORM;
        case TextureFormat::RG16             : return GL_RG16;
        case TextureFormat::RG16SNORM        : return GL_RG16_SNORM;
        case TextureFormat::R3G3B2           : return GL_R3_G3_B2;
        case TextureFormat::RGB4             : return GL_RGB4;
        case TextureFormat::RGB5             : return GL_RGB5;
        case TextureFormat::RGB8             : return GL_RGB8;
        case TextureFormat::RGB8SNORM        : return GL_RGB8_SNORM;
        case TextureFormat::RGB10            : return GL_RGB10;
        case TextureFormat::RGB12            : return GL_RGB12;
        case TextureFormat::RGB16SNORM       : return GL_RGB16_SNORM;
        case TextureFormat::RGBA2            : return GL_RGBA2;
        case TextureFormat::RGBA4            : return GL_RGBA4;
        case TextureFormat::RGB5A1           : return GL_RGB5_A1;
        case TextureFormat::RGBA8            : return GL_RGBA8;
        case TextureFormat::RGBA8SNORM       : return GL_RGBA8_SNORM;
        case TextureFormat::RGB10A2          : return GL_RGB10_A2;
        case TextureFormat::RGB10A2UI        : return GL_RGB10_A2UI;
        case TextureFormat::RGBA12           : return GL_RGBA12;
        case TextureFormat::RGBA16           : return GL_RGBA16;
        case TextureFormat::SRGB8            : return GL_SRGB8;
        case TextureFormat::SRGB8ALPHA8      : return GL_SRGB8_ALPHA8;
        case TextureFormat::R16F             : return GL_R16F;
        case TextureFormat::RG16F            : return GL_RG16F;
        case TextureFormat::RGB16F           : return GL_RGB16F;
        case TextureFormat::RGBA16F          : return GL_RGBA16F;
        case TextureFormat::R32F             : return GL_R32F;
        case TextureFormat::RG32F            : return GL_RG32F;
        case TextureFormat::RGB32F           : return GL_RGB32F;
        case TextureFormat::RGBA32F          : return GL_RGBA32F;
        case TextureFormat::R11FG11FB10F     : return GL_R11F_G11F_B10F;
        case TextureFormat::RGB9E5           : return GL_RGB9_E5;
        case TextureFormat::R8I              : return GL_R8I;
        case TextureFormat::R8UI             : return GL_R8UI;
        case TextureFormat::R16I             : return GL_R16I;
        case TextureFormat::R16UI            : return GL_R16UI;
        case TextureFormat::R32I             : return GL_R32I;
        case TextureFormat::R32UI            : return GL_R32UI;
        case TextureFormat::RG8I             : return GL_RG8I;
        case TextureFormat::RG8UI            : return GL_RG8UI;
        case TextureFormat::RG16I            : return GL_RG16I;
        case TextureFormat::RG16UI           : return GL_RG16UI;
        case TextureFormat::RG32I            : return GL_RG32I;
        case TextureFormat::RG32UI           : return GL_RG32UI;
        case TextureFormat::RGB8I            : return GL_RGB8I;
        case TextureFormat::RGB8UI           : return GL_RGB8UI;
        case TextureFormat::RGB16I           : return GL_RGB16I;
        case TextureFormat::RGB16UI          : return GL_RGB16UI;
        case TextureFormat::RGB32I           : return GL_RGB32I;
        case TextureFormat::RGB32UI          : return GL_RGB32UI;
        case TextureFormat::RGBA8I           : return GL_RGBA8I;
        case TextureFormat::RGBA8UI          : return GL_RGBA8UI;
        case TextureFormat::RGBA16I          : return GL_RGBA16I;
        case TextureFormat::RGBA16UI         : return GL_RGBA16UI;
        case TextureFormat::RGBA32I          : return GL_RGBA32I;
        case TextureFormat::RGBA32UI         : return GL_RGBA32UI;
        case TextureFormat::Depth24_Stencil8 : return GL_DEPTH24_STENCIL8;
        default                              : return GL_INVALID_ENUM;
    }
}

Texture::Texture()
    : internalFormat{TextureFormat::RGB}, imageFormat{TextureFormat::RGB8}, wrapS{TextureParameter::Repeat}, wrapT{TextureParameter::Repeat}, 
      minFilter{TextureParameter::Linear}, magFiler{TextureParameter::Linear}, hasMipmap{false} {}

Texture::Texture(const std::string& fileName, bool flipYAxis)
    : width{0}, height{0}, internalFormat{TextureFormat::RGB8}, imageFormat{TextureFormat::RGB}, wrapS{TextureParameter::Repeat}, wrapT{TextureParameter::Repeat}, 
      minFilter{TextureParameter::Linear}, magFiler{TextureParameter::Linear}, hasMipmap{false} {
    if (!Load(fileName, flipYAxis)) {
        LOG_WARN("Failed to load texture: {}.", fileName);
    }
}

Texture::Texture(Texture&& other)
    : id{other.id}, width{other.width}, height{other.height}, path{other.path}, internalFormat{other.internalFormat}, imageFormat{other.imageFormat}, 
      wrapS{other.wrapS}, wrapT{other.wrapT}, minFilter{other.minFilter}, magFiler{other.magFiler}, hasMipmap{other.hasMipmap} {
    other.id = 0;
}

Texture::~Texture() {
    Unload();
}

Texture& Texture::operator=(Texture&& other) {
    Unload();
    id = other.id;
    other.id = 0;
    width = other.width;
    height = other.height;
    path = other.path;
    internalFormat = other.internalFormat;
    imageFormat = other.imageFormat;
    wrapS = other.wrapS;
    wrapT = other.wrapT;
    minFilter = other.minFilter;
    magFiler = other.magFiler;
    hasMipmap = other.hasMipmap;
    return *this;
}

bool Texture::Load(const std::string& fileName, bool flipYAxis) {
    Unload();

    stbi_set_flip_vertically_on_load(flipYAxis);

    int channels;
    unsigned char* data{stbi_load(fileName.c_str(), &width, &height, &channels, 0)};

    if (!data) {
        LOG_WARN("Failed to load image: {}.", fileName);
        return false;
    }

    path = fileName;

    // Useful link: http://www.xphere.me/2020/06/mipmapping-effects-of-not-having-it-and-how-to-setup-in-opengl-4-5/
    if (channels == 1) { 
        internalFormat = TextureFormat::R8;
        imageFormat = TextureFormat::RED;
    }
    else if (channels == 2) {
        internalFormat = TextureFormat::RG8;
        imageFormat = TextureFormat::RG;
    }
    else if (channels == 3) {
        internalFormat = TextureFormat::RGB8;
        imageFormat = TextureFormat::RGB;
    }
    else if (channels == 4) {
        internalFormat = TextureFormat::RGBA8;
        imageFormat = TextureFormat::RGBA;
    }

#ifdef OGL_DSA
    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    // Set default parameters
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GetOpenGLTextureParameter(wrapS));
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GetOpenGLTextureParameter(wrapT));
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GetOpenGLTextureParameter(minFilter));
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GetOpenGLTextureParameter(magFiler)); 

    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
    glTextureStorage2D(id, 1, GetOpenGLTextureFormat(internalFormat), width, height);
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexSubImage2D.xhtml
    glTextureSubImage2D(id, 0, 0, 0, width, height, GetOpenGLTextureFormat(imageFormat), GL_UNSIGNED_BYTE, data);
#else 
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Set default parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFiler);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, imageFormat, GL_UNSIGNED_BYTE, data);

    // glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
#endif  // OGL_DSA

    stbi_image_free(data);

    LOG_DEBUG("Texture [{}] ({}) created.", id, fileName);

    return true;
}

void Texture::Generate(uint32_t width, uint32_t height, unsigned char* data, TextureFormat internalFormat, TextureFormat imageFormat, DataType type) {
    Unload();

    this->width = width;
    this->height = height;
    this->internalFormat = internalFormat;
    this->imageFormat = imageFormat;

    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    // Set default parameters
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GetOpenGLTextureParameter(wrapS));
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GetOpenGLTextureParameter(wrapT));
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GetOpenGLTextureParameter(minFilter));
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GetOpenGLTextureParameter(magFiler));

    glTextureStorage2D(id, 1, GetOpenGLTextureFormat(internalFormat), width, height);
    if (data)
        glTextureSubImage2D(id, 0, 0, 0, width, height, GetOpenGLTextureFormat(imageFormat), GetOpenGLDataType(type), data);
}

void Texture::Unload() {
    if (id != 0) {
        LOG_DEBUG("Texture [{}] deleted.", id);
        glDeleteTextures(1, &id);
        id = 0;
    }
}

void Texture::Use(int index) const {
#ifdef OGL_DSA
    glBindTextureUnit(index, id);
#else
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
#endif  // OGL_DSA
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture& Texture::SetWrapS(TextureParameter wrapS) {
    this->wrapS = wrapS;
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GetOpenGLTextureParameter(this->wrapS));
    return *this;
}

Texture& Texture::SetWrapT(TextureParameter wrapT) {
    this->wrapT = wrapT;
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GetOpenGLTextureParameter(this->wrapT));
    return *this;
}

Texture& Texture::SetMinFilter(TextureParameter minFilter) {
    this->minFilter = minFilter;
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GetOpenGLTextureParameter(this->minFilter));
    return *this;
}

Texture& Texture::SetMagFilter(TextureParameter magFiler) {
    this->magFiler = magFiler;
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GetOpenGLTextureParameter(this->magFiler));
    return *this;
}

Texture& Texture::GenerateMipmap() {
    hasMipmap = true;
    glGenerateTextureMipmap(id);
    return *this;
}

Texture& Texture::SetMaxAnisotropy(float anisotropy) {
    float maxAnisotropy {GetMaxAnisotropicLevel()};
    if (maxAnisotropy > 0) {
        glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
    }
    else {
        LOG_DEBUG("Anisotropy not supported.");
    }
    return *this;
}

float Texture::GetMaxAnisotropicLevel() {
    float maxAnisotropicLevel;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropicLevel);
    return maxAnisotropicLevel;
}
