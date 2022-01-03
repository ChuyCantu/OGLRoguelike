#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>

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
    bool GetShadersSource(const std::string& shader, std::unordered_map<GLenum, std::string>& outSources);
    void Unload();
    Shader& Use();
    void Unbind() const;
    bool IsNull() const { return id == 0; }

    uint32_t GetID() const { return id; }
    
    //+ Utility:
    bool HotReload();

    GLenum GetOpenGLShaderFromString(const std::string& shaderType);
    const char* GetOpenGLShaderName(GLenum shaderType);

    // Utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetBool(const std::string& name, int index, bool value) const;
    void SetBoolv(const std::string& name, int count, int* value) const;

    void SetInt(const std::string& name, int value) const;
    void SetInt(const std::string& name, int index, int value) const;
    void SetIntv(const std::string& name, int count, int* value) const;

    void SetFloat(const std::string& name, float value) const;
    void SetFloat(const std::string& name, int index, float value) const;
    void SetFloatv(const std::string& name, int count, float* value) const;

    void SetVec2(const std::string& name, const glm::vec2& vec) const;
    void SetVec2(const std::string& name, int index, const glm::vec2& vec) const;
    void SetVec2v(const std::string& name, int count, const glm::vec2* vec) const;

    void SetVec3(const std::string& name, const glm::vec3& vec) const;
    void SetVec3(const std::string& name, int index, const glm::vec3& vec) const;
    void SetVec3v(const std::string& name, int count, const glm::vec3* vec) const;

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
    bool CompileShaderFromString(const std::string& shader, GLenum shaderType, uint32_t* outShader);
    bool IsCompiled(uint32_t shader);
    bool IsValidProgram();

    void RetrieveUniformsData();

private:
    uint32_t id {0};
    std::string path;
    time_t lastModifiedTime;

    std::unordered_map<std::string, UniformInfo> uniforms;
};

#endif  // __SHADER_H__