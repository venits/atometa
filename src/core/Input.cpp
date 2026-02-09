#include "Atometa/Core/Input.h"
#include "Atometa/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Atometa {
    // Static scroll accumulator
    static float s_ScrollY = 0.0f;

    // GLFW scroll callback
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        s_ScrollY += (float)yoffset;
    }

    // Initialize scroll callback (call once from Window init)
    void Input::InitializeScrollCallback(GLFWwindow* window) {
        if (window) {
            glfwSetScrollCallback(window, ScrollCallback);
        }
    }

    bool Input::IsKeyPressed(int keycode) {
        auto window = Application::Get().GetWindow().GetNativeWindow();
        return glfwGetKey(window, keycode) == GLFW_PRESS;
    }

    bool Input::IsMouseButtonPressed(int button) {
        auto window = Application::Get().GetWindow().GetNativeWindow();
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePosition() {
        auto window = Application::Get().GetWindow().GetNativeWindow();
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return {(float)x, (float)y};
    }

    float Input::GetMouseX() { return GetMousePosition().x; }
    float Input::GetMouseY() { return GetMousePosition().y; }
    
    float Input::GetMouseScroll() {
        float scroll = s_ScrollY;
        s_ScrollY = 0.0f;  // Reset after reading
        return scroll;
    }
}