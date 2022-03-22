#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

struct UniformInfo {
    int location;
    uint32_t count;
};

class Shader {
public:
    Shader();
    explicit Shader(const std::string& shaderPath);
    Shader(Shader&& other);    
    Shader& operator=(Shader&& other);
    ~Shader();
    Shader(const Shader&) = delete;    
    Shader& operator=(const Shader&) = delete;

    bool Load(const std::string& shaderPath);
    bool GetShadersSource(const std::string& shader, std::unordered_map<uint32_t, std::string>& outSources);
    void Unload();
    void Use() const;
    void Unbind() const;
    bool IsNull() const { return id == 0; }

    uint32_t GetID() const { return id; }
    
    //+ Utility:
    bool HotReload();

    uint32_t GetOpenGLShaderFromString(const std::string& shaderType);
    const char* GetOpenGLShaderName(uint32_t shaderType);

    // Utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetBool(const std::string& name, int index, bool value) const;
    void SetBoolv(const std::string& name, int count, int* value) const;

    void SetInt(const std::string& name, int value) const;
    void SetInt(const std::string& name, int index, int value) const;
    void SetIntv(const std::string& name, int count, int* value) const;

    void SetUInt(const std::string& name, uint32_t value) const;
    void SetUInt(const std::string& name, int index, uint32_t value) const;
    void SetUIntv(const std::string& name, int count, uint32_t* value) const;

    void SetFloat(const std::string& name, float value) const;
    void SetFloat(const std::string& name, int index, float value) const;
    void SetFloatv(const std::string& name, int count, float* value) const;

    void SetIVec2(const std::string& name, const glm::ivec2& vec) const;
    void SetIVec2(const std::string& name, int index, const glm::ivec2& vec) const;
    void SetIVec2v(const std::string& name, int count, const glm::ivec2* vec) const;

    void SetVec2(const std::string& name, const glm::vec2& vec) const;
    void SetVec2(const std::string& name, int index, const glm::vec2& vec) const;
    void SetVec2v(const std::string& name, int count, const glm::vec2* vec) const;

    void SetIVec3(const std::string& name, const glm::ivec3& vec) const;
    void SetIVec3(const std::string& name, int index, const glm::ivec3& vec) const;
    void SetIVec3v(const std::string& name, int count, const glm::ivec3* vec) const;

    void SetVec3(const std::string& name, const glm::vec3& vec) const;
    void SetVec3(const std::string& name, int index, const glm::vec3& vec) const;
    void SetVec3v(const std::string& name, int count, const glm::vec3* vec) const;

    void SetIVec4(const std::string& name, const glm::ivec4& vec) const;
    void SetIVec4(const std::string& name, int index, const glm::ivec4& vec) const;
    void SetIVec4v(const std::string& name, int count, const glm::ivec4* vec) const;

    void SetVec4(const std::string& name, const glm::vec4& vec) const;
    void SetVec4(const std::string& name, int index, const glm::vec4& vec) const;
    void SetVec4v(const std::string& name, int count, const glm::vec4* vec) const;

    void SetMatrix2(const std::string& name, const glm::mat2& mat) const;
    void SetMatrix2(const std::string& name, int index, const glm::mat2& mat) const;
    void SetMatrix2v(const std::string& name, int count, const glm::mat2* mat) const;

    void SetMatrix3(const std::string& name, const glm::mat3& mat) const;
    void SetMatrix3(const std::string& name, int index, const glm::mat3& mat) const;
    void SetMatrix3v(const std::string& name, int count, const glm::mat3* mat) const;

    void SetMatrix4(const std::string& name, const glm::mat4& mat) const;
    void SetMatrix4(const std::string& name, int index, const glm::mat4& mat) const;
    void SetMatrix4v(const std::string& name, int count, const glm::mat4* mat) const;

private:
    bool CompileShaderFromString(const std::string& shader, uint32_t shaderType, uint32_t* outShader);
    bool IsCompiled(uint32_t shader) const;
    bool IsValidProgram() const;

    void RetrieveUniformsData();

private:
    uint32_t id {0};
    std::string path;
    time_t lastModifiedTime;

    std::unordered_map<std::string, UniformInfo> uniforms;
};