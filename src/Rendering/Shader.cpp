#include "Shader.hpp"

#include "Core/Log.hpp"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

Shader::Shader() {}

Shader::Shader(const std::string& shaderPath) {
    Load(shaderPath);
}

Shader::~Shader() {
    // if (id != 0)
    //     Unload();
}

bool Shader::Load(const std::string& shaderPath) {
    path = shaderPath;
    std::unordered_map<GLenum, std::string> shaderSources;
    std::ifstream file{shaderPath, std::ios::in, std::ios::binary};
    if (file.is_open()) {
        // Read file
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        std::string source{stream.str()};

        if (!GetShadersSource(source, shaderSources)) {
            LOG_WARN("Failed to load shaders from file {}.", shaderPath)
            return false;
        }

        std::vector<uint32_t> shaders(shaderSources.size());
        size_t index{0};
        for (auto& [type, source] : shaderSources) {
            uint32_t shaderID;
            if (!CompileShaderFromString(source, type, &shaderID))
                return false;
            shaders[index++] = shaderID;
        }

        id = glCreateProgram();
        for (int i{0}; i < shaders.size(); ++i)
            glAttachShader(id, shaders[i]);
        glLinkProgram(id);

        if (!IsValidProgram()) {
            id = 0;
            return false;
        }

        for (auto shader : shaders) {
            glDetachShader(id, shader);
            glDeleteShader(shader);
        }
    } else {
        LOG_WARN("Could not find/open file {}.", shaderPath);
        return false;
    }

    return true;
}

bool Shader::GetShadersSource(const std::string& shaders, std::unordered_map<GLenum, std::string>& outSources) {
    // Split shaders
    const std::string token{"#shader"};
    size_t pos{0};
    size_t offset{0};
    size_t start{0};
    size_t end{0};
    while (true) {
        pos = shaders.find(token, offset);
        if (pos == std::string::npos)
            break;
        offset = pos + token.size();
        start = shaders.find_first_not_of(" ", offset);
        end = shaders.find_first_of("\r\n", start);
        std::string type = shaders.substr(start, end - start);
        offset = end + 1;

        //? Validate shader type...
        GLenum shaderType{GetOpenGLShaderFromString(type)};
        if (shaderType == GL_NONE) {
            LOG_DEBUG("'{}' is not a valid shader type. ({})", type, path);
            return false;
        }

        //? Get shader source:
        start = shaders.find("#version", offset);
        end = shaders.find(token, offset);

        if (start == std::string::npos || start > end) {
            LOG_DEBUG("No shader source of type '{}' found. ({})", type, path);
            return false;
        }

        if (end != std::string::npos) {
            std::string shader{shaders.substr(start, end - start)};
            outSources.emplace(shaderType, shader);
            offset = end;
        } else {
            std::string shader{shaders.substr(start, shaders.size() - start)};
            outSources.emplace(shaderType, shader);
            break;
        }
    }
    return true;
}

GLenum Shader::GetOpenGLShaderFromString(const std::string& shaderType) {
    if (shaderType == "vertex" || shaderType == "vert")
        return GL_VERTEX_SHADER;
    if (shaderType == "geometry" || shaderType == "geom")
        return GL_GEOMETRY_SHADER;
    if (shaderType == "fragment" || shaderType == "frag" || shaderType == "pixel")
        return GL_FRAGMENT_SHADER;

    return GL_NONE;
}

const char* Shader::GetOpenGLShaderName(GLenum shaderType) {
    switch (shaderType) {
        case GL_VERTEX_SHADER:
            return "vertex";
        case GL_GEOMETRY_SHADER:
            return "geometry";
        case GL_FRAGMENT_SHADER:
            return "fragment";
    }

    return "invalid type";
}

void Shader::Unload() {
    glDeleteProgram(id);
    id = 0;
}

Shader& Shader::Use() {
    glUseProgram(id);
    return *this;
}

void Shader::Unbind() const {
    glUseProgram(0);
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& vec) const {
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& vec) const {
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& vec) const {
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::SetMatrix4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

bool Shader::CompileShaderFromString(const std::string& shader, GLenum shaderType, uint32_t* outShader) {
    *outShader = glCreateShader(shaderType);
    const char* code{shader.c_str()};
    glShaderSource(*outShader, 1, &code, nullptr);
    glCompileShader(*outShader);

    if (!IsCompiled(*outShader)) {
        LOG_WARN("Failed to compile shader of type: {}. ({})", GetOpenGLShaderName(shaderType), path);
        return false;
    }

    return true;
}

bool Shader::IsCompiled(uint32_t shader) {
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        LOG_WARN("Compilation of shader failed ({}): \n{}", path, infoLog);
        return false;
    }
    return true;
}

bool Shader::IsValidProgram() {
    int status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char infoLog[1024];
        glGetProgramInfoLog(id, 1024, nullptr, infoLog);
        LOG_WARN("Failed to link shader program ({}): \n{}.", path, infoLog);
        return false;
    }
    return true;
}
