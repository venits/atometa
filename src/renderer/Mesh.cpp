#include "Atometa/Renderer/Mesh.h"
#include "Atometa/Core/Logger.h"

#include <glad/glad.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Atometa {

    Mesh::Mesh() {
        m_VertexArray = CreateRef<VertexArray>();
    }

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : m_Vertices(vertices), m_Indices(indices) {
        m_VertexArray = CreateRef<VertexArray>();
        SetupMesh();
    }

    Mesh::~Mesh() {
    }

    void Mesh::SetData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
        m_Vertices = vertices;
        m_Indices = indices;
        SetupMesh();
    }

    void Mesh::Bind() const {
        m_VertexArray->Bind();
    }

    void Mesh::Unbind() const {
        m_VertexArray->Unbind();
    }

    void Mesh::Draw() const {
        m_VertexArray->Bind();
        glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
    }

    void Mesh::SetupMesh() {
        // Create vertex buffer
        m_VertexBuffer = CreateRef<VertexBuffer>(
            m_Vertices.data(),
            static_cast<uint32_t>(m_Vertices.size() * sizeof(Vertex))
        );

        // Set vertex buffer layout
        VertexBufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoords" },
            { ShaderDataType::Float3, "a_Tangent" },
            { ShaderDataType::Float3, "a_Bitangent" }
        };

        m_VertexArray->AddVertexBuffer(m_VertexBuffer, layout);

        // Create index buffer
        m_IndexBuffer = CreateRef<IndexBuffer>(
            m_Indices.data(),
            static_cast<uint32_t>(m_Indices.size())
        );

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
    }

    // ========================================================================
    // Geometry Generators
    // ========================================================================

    Mesh Mesh::CreateSphere(float radius, uint32_t sectors, uint32_t stacks) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        float sectorStep = 2 * M_PI / sectors;
        float stackStep = M_PI / stacks;

        // Generate vertices
        for (uint32_t i = 0; i <= stacks; ++i) {
            float stackAngle = M_PI / 2 - i * stackStep;
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);

            for (uint32_t j = 0; j <= sectors; ++j) {
                float sectorAngle = j * sectorStep;

                Vertex vertex;
                
                // Position
                vertex.Position.x = xy * cosf(sectorAngle);
                vertex.Position.y = xy * sinf(sectorAngle);
                vertex.Position.z = z;

                // Normal (normalized position for sphere)
                vertex.Normal = glm::normalize(vertex.Position);

                // Texture coordinates
                vertex.TexCoords.x = (float)j / sectors;
                vertex.TexCoords.y = (float)i / stacks;

                vertices.push_back(vertex);
            }
        }

        // Generate indices
        for (uint32_t i = 0; i < stacks; ++i) {
            uint32_t k1 = i * (sectors + 1);
            uint32_t k2 = k1 + sectors + 1;

            for (uint32_t j = 0; j < sectors; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (stacks - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        return Mesh(vertices, indices);
    }

    Mesh Mesh::CreateCube(float size) {
        float halfSize = size * 0.5f;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // Front face
        vertices.push_back(Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f)));

        // Back face
        vertices.push_back(Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f)));
        vertices.push_back(Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f)));

        // Top face
        vertices.push_back(Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f)));

        // Bottom face
        vertices.push_back(Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f)));

        // Right face
        vertices.push_back(Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f)));

        // Left face
        vertices.push_back(Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f)));

        // Indices for all 6 faces (2 triangles per face)
        uint32_t faceIndices[] = { 0, 1, 2, 2, 3, 0 };
        for (uint32_t i = 0; i < 6; ++i) {
            for (uint32_t j = 0; j < 6; ++j) {
                indices.push_back(i * 4 + faceIndices[j]);
            }
        }

        return Mesh(vertices, indices);
    }

    Mesh Mesh::CreateCylinder(float radius, float height, uint32_t sectors) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        float halfHeight = height * 0.5f;
        float sectorStep = 2 * M_PI / sectors;

        // Generate side vertices
        for (uint32_t i = 0; i <= 1; ++i) {
            float y = (i == 0) ? -halfHeight : halfHeight;
            
            for (uint32_t j = 0; j <= sectors; ++j) {
                float sectorAngle = j * sectorStep;
                
                Vertex vertex;
                vertex.Position.x = radius * cosf(sectorAngle);
                vertex.Position.y = y;
                vertex.Position.z = radius * sinf(sectorAngle);

                vertex.Normal.x = cosf(sectorAngle);
                vertex.Normal.y = 0.0f;
                vertex.Normal.z = sinf(sectorAngle);
                vertex.Normal = glm::normalize(vertex.Normal);

                vertex.TexCoords.x = (float)j / sectors;
                vertex.TexCoords.y = (float)i;

                vertices.push_back(vertex);
            }
        }

        // Generate side indices
        for (uint32_t i = 0; i < sectors; ++i) {
            uint32_t k1 = i;
            uint32_t k2 = i + sectors + 1;

            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }

        return Mesh(vertices, indices);
    }

    Mesh Mesh::CreatePlane(float width, float height) {
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;

        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(-halfWidth, 0.0f, -halfHeight), glm::vec3(0.0f, 1.0f, 0.0f)),
            Vertex(glm::vec3( halfWidth, 0.0f, -halfHeight), glm::vec3(0.0f, 1.0f, 0.0f)),
            Vertex(glm::vec3( halfWidth, 0.0f,  halfHeight), glm::vec3(0.0f, 1.0f, 0.0f)),
            Vertex(glm::vec3(-halfWidth, 0.0f,  halfHeight), glm::vec3(0.0f, 1.0f, 0.0f))
        };

        std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

        return Mesh(vertices, indices);
    }

}