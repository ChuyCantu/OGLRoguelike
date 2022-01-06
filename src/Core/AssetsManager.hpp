#ifndef __ASSETSMANAGER_H__
#define __ASSETSMANAGER_H__

#include "Common.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/Shader.hpp"

#include <string>
#include <unordered_map>

// TODO: Make this NOT static and initialize it in Engine so it is not destructed before Log (causing seg faults on pending logs)

class AssetsManager {
public:
    static Shader& AddShader(const std::string& name, const std::string& shaderPath);
    static Shader& GetShader(const std::string& name);
    // static const auto& GetShaders() { return shaders; }

    static Buffer* AddBuffer(const std::string& name, Ref<Buffer> buffer);
    static Buffer* GetBuffer(const std::string& name);

// private:
    static std::unordered_map<std::string, Shader> shaders;
    static std::unordered_map<std::string, Ref<Buffer>> buffers;
};

#endif // __ASSETSMANAGER_H__