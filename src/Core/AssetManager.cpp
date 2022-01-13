#include "AssetManager.hpp"

#include "Log.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexArray.hpp"

std::unordered_map<std::string, Ref<Shader>> AssetManager::shaders;
std::unordered_map<std::string, Ref<Buffer>> AssetManager::buffers;
std::unordered_map<std::string, Ref<Texture>> AssetManager::textures;
std::unordered_map<std::string, Ref<class VertexArray>> AssetManager::vertexArrays;

//+ Shaders
Ref<Shader> AssetManager::AddShader(const std::string& name, const std::string& shaderPath) {
    auto& result{shaders.emplace(name, MakeRef<Shader>(shaderPath))};
    LOGIF_DEBUG(!result.second, "A shader with the name '{}' is already registered. No insertion was done.", name);
    return result.first->second;
}

Ref<Shader> AssetManager::GetShader(const std::string& name) {
    auto iter{shaders.find(name)};
    if (iter != shaders.end())
        return iter->second;
    LOG_DEBUG("No shader with name '{}' was found.", name);
    return nullptr;
}

void AssetManager::RemoveShader(const std::string& name) {
    shaders.erase(name);
}

//+ Buffers
Ref<Buffer> AssetManager::AddBuffer(const std::string& name, Ref<Buffer> buffer) {
    auto& result {buffers.emplace(name, buffer)};
    LOGIF_DEBUG(!result.second, "A buffer with the name '{}' is already registered. No insertion was done.", name);
    return result.first->second;
}

Ref<Buffer> AssetManager::GetBuffer(const std::string& name) {
    auto iter {buffers.find(name)};
    if (iter != buffers.end())
        return iter->second;
    LOG_DEBUG("No buffer with name '{}' was found.", name);
    return nullptr;
}

void AssetManager::RemoveBuffer(const std::string& name) {
    buffers.erase(name);
}

//+ Textures
Ref<class Texture> AssetManager::AddTexture(const std::string& name, Ref<class Texture> texture) {
    auto& result{textures.emplace(name, texture)};
    LOGIF_DEBUG(!result.second, "A texture with the name '{}' is already registered. No insertion was done.", name);
    return result.first->second;
}

Ref<class Texture> AssetManager::GetTexture(const std::string& name) {
    auto iter{textures.find(name)};
    if (iter != textures.end())
        return iter->second;
    LOG_DEBUG("No texture with name '{}' was found.", name);
    return nullptr;
}

void AssetManager::RemoveTexture(const std::string& name) {
    textures.erase(name);
}

//+ Vertex Arrays
Ref<VertexArray> AssetManager::AddVertexArray(const std::string& name, Ref<VertexArray> vao) {
    auto& result{vertexArrays.emplace(name, vao)};
    LOGIF_DEBUG(!result.second, "A vertex array with the name '{}' is already registered. No insertion was done.", name);
    return result.first->second;
}

Ref<VertexArray> AssetManager::GetVertexArray(const std::string& name) {
    auto iter{vertexArrays.find(name)};
    if (iter != vertexArrays.end())
        return iter->second;
    LOG_DEBUG("No vertex array with name '{}' was found.", name);
    return nullptr;
}

void AssetManager::RemoveVertexArray(const std::string& name) {
    vertexArrays.erase(name);
}


void AssetManager::Clear() {
    shaders.clear();
    buffers.clear();
    textures.clear();
    vertexArrays.clear();
}