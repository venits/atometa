#pragma once

#include "Core.h"
#include <glm/glm.hpp>

struct GLFWwindow;  // Forward declaration

namespace Atometa {
    class Input {
    public:
        static bool IsKeyPressed(int keycode);
        static bool IsMouseButtonPressed(int button);
        static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
        static float GetMouseScroll();
        
        // Initialize scroll callback (call from Window)
        static void InitializeScrollCallback(GLFWwindow* window);
    };
}