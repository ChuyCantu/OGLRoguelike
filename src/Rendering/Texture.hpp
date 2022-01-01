#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <glad/glad.h>

#include <string>

class Texture {
   private:
    uint32_t id{0};
    int width{0};
    int height{0};

    std::string path;

    uint32_t internalFormat;  // Format of texture object
    uint32_t imageFormat;

    uint32_t wrapS;
    uint32_t wrapT;
    uint32_t minFilter;
    uint32_t magFiler;

    bool hasMipmap;

   public:
    Texture();
    Texture(const std::string& fileName, bool flipYAxis = false);
    ~Texture();

    bool Load(const std::string& fileName, bool flipYAxis = false);
    void Generate(uint32_t width, uint32_t height, unsigned char* data);
    void Unload();
    Texture& Use(int index = 0);
    void Unbind();
    bool IsNull() const { return id == 0; }

    //! These needs the texture to be currently binded!
    Texture& SetWrapS(uint32_t param);
    Texture& SetWrapT(uint32_t param);
    Texture& SetMinFilter(uint32_t param);
    Texture& SetMagFilter(uint32_t param);
    Texture& GenerateMipmap();

    uint32_t GetID() const { return id; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    const std::string& GetPath() const { return path; }
    uint32_t GetInternalFormat() const { return internalFormat; }
    uint32_t GetImageFormat() const { return imageFormat; }
    uint32_t GetWrapS() const { return wrapS; }
    uint32_t GetWrapT() const { return wrapT; }
    uint32_t GetMinFilter() const { return minFilter; }
    uint32_t GetMagFiler() const { return magFiler; }
    bool HasMipmap() const { return hasMipmap; }
};

#endif  // __TEXTURE_H__