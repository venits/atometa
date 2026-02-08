#pragma once

#include "Atometa/Core/Core.h"

struct GLFWwindow;

namespace Atometa {

    class ImGuiLayer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void OnAttach(GLFWwindow* window);
        void OnDetach();

        void Begin();
        void End();

        void ShowDemoWindow();
        void ShowViewportWindow(bool* pOpen = nullptr);
        void ShowPropertiesWindow(bool* pOpen = nullptr);
        void ShowSceneHierarchyWindow(bool* pOpen = nullptr);
        void ShowPerformanceWindow(bool* pOpen = nullptr);

    private:
        void SetupStyle();
    };

}