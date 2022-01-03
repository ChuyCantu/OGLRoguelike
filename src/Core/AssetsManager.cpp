#include "AssetsManager.hpp"

std::unordered_map<std::string, Shader> AssetsManager::shaders;

Shader& AssetsManager::AddShader(const std::string& name, const std::string& shaderPath) {
    auto& shader {shaders.emplace(name, Shader{shaderPath})};
    return shader.first->second;
}

// TODO: Fix this
Shader& AssetsManager::GetShader(const std::string& name) {
    auto& iter {shaders.find(name)};
    if (iter != shaders.end()) 
        return iter->second;
    else
        return Shader{};
}
