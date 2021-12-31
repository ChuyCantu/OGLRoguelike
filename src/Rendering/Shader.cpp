#include "Shader.hpp"

#include <SDL.h>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

Shader::Shader() {}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    if (!Load(vertexPath, fragmentPath))
        SDL_Log("Failed to load shaders: %s, %s.", vertexPath.c_str(), fragmentPath.c_str());
}

Shader::Shader(const std::string& shaderPath) {
    if (!Load(shaderPath))
        SDL_Log("Failed to load shader: %s.", shaderPath.c_str());
}

Shader::~Shader() {
    // if (id != 0)
    //     Unload();
}

bool Shader::Load(const std::string& vertexPath, const std::string& fragmentPath) {
    if (id != 0)
        Unload();

    uint32_t vertexShader;
    uint32_t fragmentShader;

    if (!CompileShaderFromFile(vertexPath, GL_VERTEX_SHADER, vertexShader) ||
        !CompileShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER, fragmentShader))
        return false;

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!IsValidProgram()) {
        id = 0;
        return false;
    }

    return true;
}

bool Shader::Load(const std::string& shaderPath) {
    std::unordered_map<GLenum, std::string> shaderSources;
    std::ifstream file{shaderPath, std::ios::in, std::ios::binary};
    if (file.is_open()) {
        // Read file
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        std::string source{stream.str()};

        if (!GetShadersSource(source, shaderSources)) {
            SDL_Log("Failed to load shaders from file %s.", shaderPath.c_str());
            return false;
        }

        std::vector<GLuint> shaders(shaderSources.size());
        size_t index{0};
        for (auto& [type, source] : shaderSources) {
            GLuint shaderID;
            if (!CompileShaderFromString(source, type, shaderID))
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
        SDL_Log("Could not find/open file %s.", shaderPath.c_str());
        return false;
    }

    // SDL_Log("Shader created: [%i] %s", id, shaderPath.c_str());

    return true;
}

bool Shader::GetShadersSource(const std::string& shaders, std::unordered_map<GLenum, std::string>& sources) {
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
            SDL_Log("%s is not a valid shader type.", type.c_str());
            return false;
        }

        //? Get shader source:
        start = shaders.find("#version", offset);
        end = shaders.find(token, offset);

        if (start == std::string::npos || start > end) {
            SDL_Log("No shader source of type %s found.", type.c_str());
            return false;
        }

        if (end != std::string::npos) {
            std::string shader{shaders.substr(start, end - start)};
            sources.emplace(shaderType, shader);
            offset = end;
        } else {
            std::string shader{shaders.substr(start, shaders.size() - start)};
            sources.emplace(shaderType, shader);
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
    // SDL_Log("Shader deleted: [%i]", id);
    glDeleteProgram(id);
    id = 0;
}

void Shader::Use() const {
    glUseProgram(id);
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

bool Shader::CompileShaderFromFile(const std::string& fileName, GLenum shaderType, GLuint& outShader) {
    std::ifstream file{fileName};
    if (file.is_open()) {
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        std::string codes{stream.str()};
        const char* code{codes.c_str()};

        outShader = glCreateShader(shaderType);
        glShaderSource(outShader, 1, &(code), nullptr);
        glCompileShader(outShader);

        if (!IsCompiled(outShader)) {
            SDL_Log("Failed to compile shader: %s.", fileName.c_str());
            return false;
        }
    } else {
        SDL_Log("Could not find/open shader file: %s.", fileName.c_str());
        return false;
    }
    return true;
}

bool Shader::CompileShaderFromString(const std::string& shader, GLenum shaderType, GLuint& outShader) {
    outShader = glCreateShader(shaderType);
    const char* code{shader.c_str()};
    glShaderSource(outShader, 1, &code, nullptr);
    glCompileShader(outShader);

    if (!IsCompiled(outShader)) {
        SDL_Log("Failed to compile shader of type: %s.", GetOpenGLShaderName(shaderType));
        return false;
    }

    return true;
}

bool Shader::IsCompiled(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        SDL_Log("Compilation of shader failed: \n%s.", infoLog);
        return false;
    }
    return true;
}

bool Shader::IsValidProgram() {
    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char infoLog[1024];
        glGetProgramInfoLog(id, 1024, nullptr, infoLog);
        SDL_Log("Failed to link shader program: \n%s.", infoLog);
        return false;
    }
    return true;
}
