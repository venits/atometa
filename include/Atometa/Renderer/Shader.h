#pragma once

#include "Atometa/Core/Core.h"
#include <string>
#include <glm/glm.hpp>

namespace Atometa {

    class Shader {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        // Utility uniform functions
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

        uint32_t GetRendererID() const { return m_RendererID; }

    private:
        uint32_t CompileShader(uint32_t type, const std::string& source);
        std::string ReadFile(const std::string& filepath);
        int GetUniformLocation(const std::string& name);

    private:
        uint32_t m_RendererID;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };

}