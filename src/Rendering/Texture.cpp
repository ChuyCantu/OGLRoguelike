#include "Texture.hpp"

#include "Core/Log.hpp"

#include <stb_image.h>

#define OGL_DSA

Texture::Texture()
    : internalFormat{GL_RGB}, imageFormat{GL_RGB8}, wrapS{GL_REPEAT}, wrapT{GL_REPEAT}, minFilter{GL_LINEAR}, magFiler{GL_LINEAR}, hasMipmap{false} {}

Texture::Texture(const std::string& fileName, bool flipYAxis)
    : width{0}, height{0}, internalFormat{GL_RGB8}, imageFormat{GL_RGB}, wrapS{GL_REPEAT}, wrapT{GL_REPEAT}, minFilter{GL_LINEAR}, magFiler{GL_LINEAR}, hasMipmap{false} {
    if (!Load(fileName, flipYAxis))
        LOG_WARN("Failed to load texture: {}.", fileName);
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
        internalFormat = GL_R8;
        imageFormat = GL_RED;
    }
    else if (channels == 2) {
        internalFormat = GL_RG8;
        imageFormat = GL_RG;
    }
    else if (channels == 3) {
        internalFormat = GL_RGB8;
        imageFormat = GL_RGB;
    }
    else if (channels == 4) {
        internalFormat = GL_RGBA8;
        imageFormat = GL_RGBA;
    }

#ifdef OGL_DSA
    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    // Set default parameters
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrapS);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrapT);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, magFiler);

    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
    glTextureStorage2D(id, 1, internalFormat, width, height);
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexSubImage2D.xhtml
    glTextureSubImage2D(id, 0, 0, 0, width, height, imageFormat, GL_UNSIGNED_BYTE, data);
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

void Texture::Generate(uint32_t width, uint32_t height, unsigned char* data) {
    Unload();

    this->width = width;
    this->height = height;

    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    // Set default parameters
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrapS);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrapT);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, magFiler);

    glTextureStorage2D(id, 1, internalFormat, width, height);
    if (data)
        glTextureSubImage2D(id, 0, 0, 0, width, height, imageFormat, GL_UNSIGNED_BYTE, data);
}

void Texture::Unload() {
    if (id != 0) {
        LOG_DEBUG("Texture [{}] deleted.", id);
        glDeleteTextures(1, &id);
        id = 0;
    }
}

Texture& Texture::Use(int index) {
#ifdef OGL_DSA
    glBindTextureUnit(index, id);
#else
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
#endif  // OGL_DSA
    return *this;
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture& Texture::SetWrapS(uint32_t wrapS) {
    this->wrapS = wrapS;
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, this->wrapS);
    return *this;
}

Texture& Texture::SetWrapT(uint32_t wrapT) {
    this->wrapT = wrapT;
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, this->wrapT);
    return *this;
}

Texture& Texture::SetMinFilter(uint32_t minFilter) {
    this->minFilter = minFilter;
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, this->minFilter);
    return *this;
}

Texture& Texture::SetMagFilter(uint32_t magFiler) {
    this->magFiler = magFiler;
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, this->magFiler);
    return *this;
}

Texture& Texture::GenerateMipmap() {
    hasMipmap = true;
    glGenerateTextureMipmap(id);
    return *this;
}

Texture& Texture::SetMaxAnisotropy(float anisotropy) {
    float maxAnisotropy {GetMaxAnisotropicLevel()};
    if (maxAnisotropy > 0)
        glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
    else
        LOG_DEBUG("Anisotropy not supported.");

    return *this;
}

float Texture::GetMaxAnisotropicLevel() {
    float maxAnisotropicLevel;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropicLevel);
    return maxAnisotropicLevel;
}
