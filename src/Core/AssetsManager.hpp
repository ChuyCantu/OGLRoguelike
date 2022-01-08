#ifndef __ASSETSMANAGER_H__
#define __ASSETSMANAGER_H__

#include "Common.hpp"
#include <string>
#include <unordered_map>

class AssetsManager {
public:  
    static Ref<class Shader> AddShader(const std::string& name, const std::string& shaderPath);
    static Ref<class Shader> GetShader(const std::string& name);
    static void RemoveShader(const std::string& name);

    static Ref<class Buffer> AddBuffer(const std::string& name, Ref<class Buffer> buffer);
    static Ref<class Buffer> GetBuffer(const std::string& name);
    static void RemoveBuffer(const std::string& name);

    static Ref<class Texture> AddTexture(const std::string& name, Ref<class Texture> texture);
    static Ref<class Texture> GetTexture(const std::string& name);
    static void RemoveTexture(const std::string& name);

    static void Clear();

    static auto& GetShaders() { return shaders; }
    static auto& GetBuffers() { return buffers; }

private:
    static std::unordered_map<std::string, Ref<class Shader>> shaders;
    static std::unordered_map<std::string, Ref<class Buffer>> buffers;
    static std::unordered_map<std::string, Ref<class Texture>> textures;
};

#endif // __ASSETSMANAGER_H__