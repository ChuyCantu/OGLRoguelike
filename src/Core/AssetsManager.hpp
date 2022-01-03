#ifndef __ASSETSMANAGER_H__
#define __ASSETSMANAGER_H__

#include "Common.hpp"
#include "Rendering/Shader.hpp"

#include <string>
#include <unordered_map>

class AssetsManager {
public:
    static Shader& AddShader(const std::string& name, const std::string& shaderPath);
    static Shader& GetShader(const std::string& name);
    // static const auto& GetShaders() { return shaders; }


// private:
    static std::unordered_map<std::string, Shader> shaders;
};

#endif // __ASSETSMANAGER_H__