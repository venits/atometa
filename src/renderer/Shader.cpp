#include "Atometa/Renderer/Shader.h"
#include "Atometa/Core/Logger.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

namespace Atometa {

    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexSource = ReadFile(vertexPath);
        std::string fragmentSource = ReadFile(fragmentPath);

        uint32_t vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
        uint32_t fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

        // Link shaders
        m_RendererID = glCreateProgram();
        glAttachShader(m_RendererID, vertexShader);
        glAttachShader(m_RendererID, fragmentShader);
        glLinkProgram(m_RendererID);

        // Check for linking errors
        int success;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_RendererID, 512, nullptr, infoLog);
            ATOMETA_ERROR("Shader linking failed: ", infoLog);
        }

        // Delete shaders as they're linked into program now
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        ATOMETA_INFO("Shader created successfully: ", vertexPath);
    }

    Shader::~Shader() {
        glDeleteProgram(m_RendererID);
    }

    void Shader::Bind() const {
        glUseProgram(m_RendererID);
    }

    void Shader::Unbind() const {
        glUseProgram(0);
    }

    void Shader::SetInt(const std::string& name, int value) {
        glUniform1i(GetUniformLocation(name), value);
    }

    void Shader::SetFloat(const std::string& name, float value) {
        glUniform1f(GetUniformLocation(name), value);
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
        glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
        glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    uint32_t Shader::CompileShader(uint32_t type, const std::string& source) {
        uint32_t shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        // Check for compilation errors
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            const char* shaderType = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
            ATOMETA_ERROR(shaderType, " shader compilation failed: ", infoLog);
        }

        return shader;
    }

    std::string Shader::ReadFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            ATOMETA_ERROR("Failed to open shader file: ", filepath);
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    int Shader::GetUniformLocation(const std::string& name) {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
            return m_UniformLocationCache[name];
        }

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            ATOMETA_WARN("Uniform '", name, "' not found in shader");
        }

        m_UniformLocationCache[name] = location;
        return location;
    }

}