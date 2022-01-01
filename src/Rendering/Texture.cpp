#include "Texture.hpp"

#include "Core/Log.hpp"

#include <stb_image.h>

Texture::Texture()
    : internalFormat{GL_RGB}, imageFormat{GL_RGB}, wrapS{GL_REPEAT}, wrapT{GL_REPEAT}, minFilter{GL_LINEAR}, magFiler{GL_LINEAR}, hasMipmap{false} {}

Texture::Texture(const std::string& fileName, bool flipYAxis)
    : width{0}, height{0}, internalFormat{GL_RGB}, imageFormat{GL_RGB}, wrapS{GL_REPEAT}, wrapT{GL_REPEAT}, minFilter{GL_LINEAR}, magFiler{GL_LINEAR}, hasMipmap{false} {
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
    if (id != 0)
        Unload();

    stbi_set_flip_vertically_on_load(flipYAxis);

    int channels;
    unsigned char* data{stbi_load(fileName.c_str(), &width, &height, &channels, 0)};

    if (!data) {
        LOG_WARN("Failed to load image: {}.", fileName);
        return false;
    }

    path = fileName;

    if (channels == 1)
        imageFormat = GL_RED;
    else if (channels == 4)
        imageFormat = GL_RGBA;

    internalFormat = imageFormat;

    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    // Set default parameters
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrapS);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrapT);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, magFiler);

    // float maxAnisotropicLevel;
    // glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropicLevel);
    // if (maxAnisotropicLevel > 0)
    //     glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropicLevel);
    // else
    //     LOG_DEBUG("Anisotropy not supported.");

    glTextureStorage2D(id, 1, internalFormat, width, height);
    glTextureSubImage2D(id, 0, 0, 0, width, height, imageFormat, GL_UNSIGNED_BYTE, data);
    // glGenerateTextureMipmap(id);

    stbi_image_free(data);

    LOG_DEBUG("Texture [{}] ({}) created.", id, fileName);

    return true;
}

void Texture::Generate(uint32_t width, uint32_t height, unsigned char* data) {
    if (id != 0)
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
    glTextureSubImage2D(id, 0, 0, 0, width, height, imageFormat, GL_UNSIGNED_BYTE, data);
    // glGenerateTextureMipmap(id);
}

void Texture::Unload() {
    if (id != 0) {
        LOG_DEBUG("Texture [{}] deleted.", id);
        glDeleteTextures(1, &id);
        id = 0;
    }
}

Texture& Texture::Use(int index) {
    glBindTextureUnit(index, id);
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