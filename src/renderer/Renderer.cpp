#include "Atometa/Renderer/Renderer.h"
#include "Atometa/Core/Logger.h"

#include <glad/glad.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Atometa {

    uint32_t Renderer::s_SphereVAO = 0;
    uint32_t Renderer::s_SphereVBO = 0;
    uint32_t Renderer::s_SphereEBO = 0;
    uint32_t Renderer::s_SphereIndexCount = 0;

    void Renderer::Init() {
        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        
        // Enable backface culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        // Create default sphere for atom rendering
        CreateSphere(1.0f, 32, 16);
        
        ATOMETA_INFO("Renderer initialized");
    }

    void Renderer::Shutdown() {
        DestroySphere();
        ATOMETA_INFO("Renderer shutdown");
    }

    void Renderer::Clear(const glm::vec4& color) {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        glViewport(x, y, width, height);
    }

    void Renderer::CreateSphere(float radius, int sectors, int stacks) {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        float sectorStep = 2 * M_PI / sectors;
        float stackStep = M_PI / stacks;

        // Generate vertices
        for (int i = 0; i <= stacks; ++i) {
            float stackAngle = M_PI / 2 - i * stackStep;
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectors; ++j) {
                float sectorAngle = j * sectorStep;

                // Position
                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Normal
                float nx = x / radius;
                float ny = y / radius;
                float nz = z / radius;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
            }
        }

        // Generate indices
        for (int i = 0; i < stacks; ++i) {
            int k1 = i * (sectors + 1);
            int k2 = k1 + sectors + 1;

            for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
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

        s_SphereIndexCount = static_cast<uint32_t>(indices.size());

        // Create VAO, VBO, EBO
        glGenVertexArrays(1, &s_SphereVAO);
        glGenBuffers(1, &s_SphereVBO);
        glGenBuffers(1, &s_SphereEBO);

        glBindVertexArray(s_SphereVAO);

        glBindBuffer(GL_ARRAY_BUFFER, s_SphereVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_SphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

        // Normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);

        ATOMETA_INFO("Sphere created with ", sectors, " sectors and ", stacks, " stacks");
    }

    void Renderer::DrawSphere() {
        glBindVertexArray(s_SphereVAO);
        glDrawElements(GL_TRIANGLES, s_SphereIndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Renderer::DestroySphere() {
        glDeleteVertexArrays(1, &s_SphereVAO);
        glDeleteBuffers(1, &s_SphereVBO);
        glDeleteBuffers(1, &s_SphereEBO);
    }

}