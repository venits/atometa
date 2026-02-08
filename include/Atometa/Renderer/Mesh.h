#pragma once

#include "Atometa/Core/Core.h"
#include "VertexArray.h"
#include "Buffer.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Atometa {

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;

        Vertex()
            : Position(0.0f), Normal(0.0f), TexCoords(0.0f),
              Tangent(0.0f), Bitangent(0.0f) {}

        Vertex(const glm::vec3& pos, const glm::vec3& norm)
            : Position(pos), Normal(norm), TexCoords(0.0f),
              Tangent(0.0f), Bitangent(0.0f) {}
    };

    class Mesh {
    public:
        Mesh();
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();

        void SetData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        
        void Bind() const;
        void Unbind() const;
        void Draw() const;

        const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

        // Geometry generation
        static Mesh CreateSphere(float radius, uint32_t sectors, uint32_t stacks);
        static Mesh CreateCube(float size);
        static Mesh CreateCylinder(float radius, float height, uint32_t sectors);
        static Mesh CreatePlane(float width, float height);

    private:
        void SetupMesh();

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;

        Ref<VertexArray> m_VertexArray;
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;
    };

}