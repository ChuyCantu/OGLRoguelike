#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
private:
    // Program ID
    uint32_t id {0};
    std::string path;

public:
    Shader();
    explicit Shader(const std::string& shaderPath);
    ~Shader();

    bool Load(const std::string& shaderPath);
    bool GetShadersSource(const std::string& shader, std::unordered_map<GLenum, std::string>& outSources);
    void Unload();
    Shader& Use();
    void Unbind() const;
    bool IsNull() const { return id == 0; }

    uint32_t GetID() const { return id; }

    GLenum GetOpenGLShaderFromString(const std::string& shaderType);
    const char* GetOpenGLShaderName(GLenum shaderType);

    // Utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& vec) const;
    void SetVec3(const std::string& name, const glm::vec3& vec) const;
    void SetVec4(const std::string& name, const glm::vec4& vec) const;
    void SetMatrix4(const std::string& name, const glm::mat4& mat) const;

private:
    bool CompileShaderFromString(const std::string& shader, GLenum shaderType, uint32_t* outShader);
    bool IsCompiled(uint32_t shader);
    bool IsValidProgram();
};

#endif  // __SHADER_H__