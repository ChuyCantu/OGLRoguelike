#include "Texture.hpp"

// #define STB_IMAGE_IMPLEMENTATION
#include <SDL.h>
#include <stb_image.h>

Texture::Texture()
    : internalFormat{GL_RGB}, imageFormat{GL_RGB}, wrapS{GL_REPEAT}, wrapT{GL_REPEAT}, minFilter{GL_LINEAR}, magFiler{GL_LINEAR}, hasMipmap{false} {}

Texture::Texture(const std::string& fileName, bool flipYAxis)
    : width{0}, height{0}, internalFormat{GL_RGB}, imageFormat{GL_RGB}, wrapS{GL_REPEAT}, wrapT{GL_REPEAT}, minFilter{GL_LINEAR}, magFiler{GL_LINEAR}, hasMipmap{false} {
    if (!Load(fileName, flipYAxis))
        SDL_Log("Failed to load texture: %s.", fileName.c_str());
}

Texture::~Texture() {
    // if (id != 0)
    //     Unload();
}

bool Texture::Load(const std::string& fileName, bool flipYAxis) {
    if (id != 0)
        Unload();

    stbi_set_flip_vertically_on_load(flipYAxis);

    int channels;
    unsigned char* data{stbi_load(fileName.c_str(), &width, &height, &channels, 0)};

    if (!data) {
        SDL_Log("Failed to load image: %s.", fileName.c_str());
        return false;
    }

    path = fileName;

    if (channels == 1)
        imageFormat = GL_RED;
    else if (channels == 4)
        imageFormat = GL_RGBA;

    internalFormat = imageFormat;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Set default parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFiler);

    float maxAnisotropicLevel;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropicLevel);
    if (maxAnisotropicLevel > 0)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropicLevel);
    else
        SDL_Log("Anisotropy not supported.");

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    // SDL_Log("Texture created: [%i] %s", id, fileName.c_str());

    return true;
}

void Texture::Generate(uint32_t width, uint32_t height, unsigned char* data) {
    if (id != 0)
        Unload();

    this->width = width;
    this->height = height;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);

    // Set default parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFiler);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Unload() {
    // SDL_Log("Texture deleted: [%i]", id);
    glDeleteTextures(1, &id);
    id = 0;
}

Texture& Texture::Use(int index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, id);

    return *this;
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture& Texture::SetWrapS(uint32_t wrapS) {
    this->wrapS = wrapS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
    return *this;
}

Texture& Texture::SetWrapT(uint32_t wrapT) {
    this->wrapT = wrapT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
    return *this;
}

Texture& Texture::SetMinFilter(uint32_t minFilter) {
    this->minFilter = minFilter;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->minFilter);
    return *this;
}

Texture& Texture::SetMagFilter(uint32_t magFiler) {
    this->magFiler = magFiler;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->magFiler);
    return *this;
}

Texture& Texture::GenerateMipmap() {
    hasMipmap = true;
    glGenerateMipmap(GL_TEXTURE_2D);
    return *this;
}