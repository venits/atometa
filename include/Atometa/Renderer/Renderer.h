#pragma once

#include "Atometa/Core/Core.h"
#include <glm/glm.hpp>
#include <vector>

namespace Atometa {

    class Renderer {
    public:
        static void Init();
        static void Shutdown();
        
        static void Clear(const glm::vec4& color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        
        // Sphere rendering for atoms
        static void CreateSphere(float radius, int sectors, int stacks);
        static void DrawSphere();
        static void DestroySphere();
        
    private:
        static uint32_t s_SphereVAO;
        static uint32_t s_SphereVBO;
        static uint32_t s_SphereEBO;
        static uint32_t s_SphereIndexCount;
    };

}