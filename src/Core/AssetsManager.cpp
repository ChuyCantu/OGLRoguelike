#include "AssetsManager.hpp"
#include "Log.hpp"

std::unordered_map<std::string, Shader> AssetsManager::shaders;
std::unordered_map<std::string, Ref<Buffer>> AssetsManager::buffers;

Shader& AssetsManager::AddShader(const std::string& name, const std::string& shaderPath) {
    auto& result {shaders.emplace(name, Shader{shaderPath})};
    LOGIF_DEBUG(result.second, "A shader with the name '{}' is already registered. No insertion was done.", name);
    return result.first->second;
}

// TODO: Fix this
Shader& AssetsManager::GetShader(const std::string& name) {
    auto iter {shaders.find(name)};
    if (iter != shaders.end()) 
        return iter->second;
    LOG_DEBUG("No buffer with name '{}' was found.", name);
    return Shader{};
}

Buffer* AssetsManager::AddBuffer(const std::string& name, Ref<Buffer> buffer) {
    auto& result {buffers.emplace(name, buffer)};
    LOGIF_DEBUG(result.second, "A buffer with the name '{}' is already registered. No insertion was done.", name);
    return result.first->second.get();
}

Buffer* AssetsManager::GetBuffer(const std::string& name) {
    auto iter {buffers.find(name)};
    if (iter != buffers.end())
        return iter->second.get();
    LOG_DEBUG("No buffer with name '{}' was found.", name);
    return nullptr;
}
