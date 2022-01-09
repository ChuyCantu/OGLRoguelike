#include "Shader.hpp"

#include "Core/Log.hpp"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

// TODO: Move this to a file system class
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#endif
#ifdef WIN32 
#define stat _stat
#endif

Shader::Shader() {}

Shader::Shader(const std::string& shaderPath) {
    Load(shaderPath);
}


// TODO: Make it so the shader is first destroyed before getting "other"s values. (Also with textures...)
Shader::Shader(Shader&& other) 
    : id{other.id}, path{std::move(other.path)}, lastModifiedTime{other.lastModifiedTime}, uniforms{std::move(other.uniforms)} {
    other.id = 0;
}

Shader::~Shader() {
    Unload();
}

Shader& Shader::operator=(Shader&& other) {
    Unload();
    id = other.id;
    other.id = 0;
    path = std::move(other.path);
    lastModifiedTime = other.lastModifiedTime;
    uniforms = std::move(other.uniforms);
    return *this;
}

bool Shader::Load(const std::string& shaderPath) {
    Unload();

    path = shaderPath;
    std::unordered_map<GLenum, std::string> shaderSources;
    std::ifstream file{shaderPath, std::ios::in, std::ios::binary};
    if (file.is_open()) {
        // Check last modified date
        // TODO: Move to file system
        struct stat result;
        if (stat(path.c_str(), &result) == 0) {
            lastModifiedTime = result.st_mtime;
        }

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

    RetrieveUniformsData();

    LOG_DEBUG("Shader [{}] created.", id);

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

uint32_t Shader::GetOpenGLShaderFromString(const std::string& shaderType) {
    if (shaderType == "vertex" || shaderType == "vert")
        return GL_VERTEX_SHADER;
    if (shaderType == "geometry" || shaderType == "geom")
        return GL_GEOMETRY_SHADER;
    if (shaderType == "fragment" || shaderType == "frag" || shaderType == "pixel")
        return GL_FRAGMENT_SHADER;

    return GL_NONE;
}

const char* Shader::GetOpenGLShaderName(uint32_t shaderType) {
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
    if (id != 0) {
        LOG_DEBUG("Shader [{}] deleted.", id);
        glDeleteProgram(id);
        id = 0;
    }
}

void Shader::Use() const {
    glUseProgram(id);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

bool Shader::HotReload() {
    if (id != 0 && !path.empty()) {
        // https://stackoverflow.com/questions/40504281/c-how-to-check-the-last-modified-time-of-a-file
        struct stat result;
        if (stat(path.c_str(), &result) == 0) {
            auto mod_time = result.st_mtime;
            double diff {difftime(mod_time, lastModifiedTime)};
            if (diff != 0.0) {
                lastModifiedTime = mod_time;
                Shader newShader;
                if (newShader.Load(path)) {
                    Unload();
                    id = newShader.id;
                    newShader.id = 0;
                    uniforms = std::move(newShader.uniforms);
                    return true;
                }
            }
        }

        
    }
    return false;
}

//+ ===========================================================================================================
//+ ===========================================================================================================

const char* index_0_str { "[0]" };

void Shader::SetBool(const std::string& name, bool value) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform1i(id, iter->second.location, value);
}

void Shader::SetBool(const std::string& name, int index, bool value) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform1i(id, iter->second.location + index, value);
}

void Shader::SetBoolv(const std::string& name, int count, int* value) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform1iv(id, iter->second.location, count, value);
}

void Shader::SetInt(const std::string& name, int value) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform1i(id, iter->second.location, value);
}

void Shader::SetInt(const std::string& name, int index, int value) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform1i(id, iter->second.location + index, value);
}

void Shader::SetIntv(const std::string& name, int count, int* value) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform1iv(id, iter->second.location, count, value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform1f(id, iter->second.location, value);
}

void Shader::SetFloat(const std::string& name, int index, float value) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform1f(id, iter->second.location + index, value);
}

void Shader::SetFloatv(const std::string& name, int count, float* value) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform1fv(id, iter->second.location, count, value);
}

void Shader::SetIVec2(const std::string& name, const glm::ivec2& vec) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform2iv(id, iter->second.location, 1, glm::value_ptr(vec));
}

void Shader::SetIVec2(const std::string& name, int index, const glm::ivec2& vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform2iv(id, iter->second.location + index, 1, glm::value_ptr(vec));
}

void Shader::SetIVec2v(const std::string& name, int count, const glm::ivec2* vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform2iv(id, iter->second.location, count, glm::value_ptr(vec[0]));
}

void Shader::SetVec2(const std::string& name, const glm::vec2& vec) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform2fv(id, iter->second.location, 1, glm::value_ptr(vec));
}

void Shader::SetVec2(const std::string& name, int index, const glm::vec2& vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform2fv(id, iter->second.location + index, 1, glm::value_ptr(vec));
}

void Shader::SetVec2v(const std::string& name, int count, const glm::vec2* vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform2fv(id, iter->second.location, count, glm::value_ptr(vec[0]));
}

void Shader::SetIVec3(const std::string& name, const glm::ivec3& vec) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform3iv(id, iter->second.location, 1, glm::value_ptr(vec));
}

void Shader::SetIVec3(const std::string& name, int index, const glm::ivec3& vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform3iv(id, iter->second.location + index, 1, glm::value_ptr(vec));
}

void Shader::SetIVec3v(const std::string& name, int count, const glm::ivec3* vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform3iv(id, iter->second.location, count, glm::value_ptr(vec[0]));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& vec) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform3fv(id, iter->second.location, 1, glm::value_ptr(vec));
}

void Shader::SetVec3(const std::string& name, int index, const glm::vec3& vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform3fv(id, iter->second.location + index, 1, glm::value_ptr(vec));
}

void Shader::SetVec3v(const std::string& name, int count, const glm::vec3* vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform3fv(id, iter->second.location, count, glm::value_ptr(vec[0]));
}

void Shader::SetIVec4(const std::string& name, const glm::ivec4& vec) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform4iv(id, iter->second.location, 1, glm::value_ptr(vec));
}

void Shader::SetIVec4(const std::string& name, int index, const glm::ivec4& vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform4iv(id, iter->second.location + index, 1, glm::value_ptr(vec));
}

void Shader::SetIVec4v(const std::string& name, int count, const glm::ivec4* vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform4iv(id, iter->second.location, count, glm::value_ptr(vec[0]));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& vec) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniform4fv(id, iter->second.location, 1, glm::value_ptr(vec));
}

void Shader::SetVec4(const std::string& name, int index, const glm::vec4& vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform4fv(id, iter->second.location + index, 1, glm::value_ptr(vec));
}

void Shader::SetVec4v(const std::string& name, int count, const glm::vec4* vec) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniform4fv(id, iter->second.location, count, glm::value_ptr(vec[0]));
}

void Shader::SetMatrix2(const std::string& name, const glm::mat2& mat) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniformMatrix2fv(id, iter->second.location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMatrix2(const std::string& name, int index, const glm::mat2& mat) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniformMatrix2fv(id, iter->second.location + index, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMatrix2v(const std::string& name, int count, const glm::mat2* mat) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniformMatrix2fv(id, iter->second.location, count, GL_FALSE, glm::value_ptr(mat[0]));
}

void Shader::SetMatrix3(const std::string& name, const glm::mat3& mat) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniformMatrix3fv(id, iter->second.location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMatrix3(const std::string& name, int index, const glm::mat3& mat) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniformMatrix3fv(id, iter->second.location + index, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMatrix3v(const std::string& name, int count, const glm::mat3* mat) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniformMatrix3fv(id, iter->second.location, count, GL_FALSE, glm::value_ptr(mat[0]));
}

void Shader::SetMatrix4(const std::string& name, const glm::mat4& mat) const {
    auto iter{uniforms.find(name)};
    if (iter != uniforms.end())
        glProgramUniformMatrix4fv(id, iter->second.location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMatrix4(const std::string& name, int index, const glm::mat4& mat) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniformMatrix4fv(id, iter->second.location + index, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMatrix4v(const std::string& name, int count, const glm::mat4* mat) const {
    auto iter{uniforms.find(name + index_0_str)};
    if (iter != uniforms.end())
        glProgramUniformMatrix4fv(id, iter->second.location, count, GL_FALSE, glm::value_ptr(mat[0]));
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

bool Shader::IsCompiled(uint32_t shader) const {
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

bool Shader::IsValidProgram() const {
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

void Shader::RetrieveUniformsData() {
    GLint uniformCount {0};
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);

    if (uniformCount != 0) {
        GLint maxNameLength = 0;
        GLsizei length = 0;
        GLsizei count = 0;
        GLenum type = GL_NONE;
        glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

        auto uniformName = std::make_unique<char[]>(maxNameLength);

        for (GLint i = 0; i < uniformCount; ++i) {
            glGetActiveUniform(id, i, maxNameLength, &length, &count, &type, uniformName.get());

            UniformInfo uniformInfo = {};
            uniformInfo.location = glGetUniformLocation(id, uniformName.get());
            uniformInfo.count = count;

            uniforms.emplace(std::make_pair(std::string(uniformName.get(), length), uniformInfo));
            //uniforms.emplace(std::string(uniformName.get(), length), uniformInfo);
        }
    }

    std::string debugUniforms;
    for (auto& [name, info] : uniforms) {
        debugUniforms += fmt::format("[{}] loc: {}, count: {}.\n", name, info.location, info.count);
    }
    
    // LOG_DEBUG("\nShader '{}' uniforms:\n" + debugUniforms, path);
}