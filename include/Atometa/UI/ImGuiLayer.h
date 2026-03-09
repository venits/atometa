#pragma once

#include "Atometa/Core/Core.h"

struct GLFWwindow;

namespace Atometa {

    class Scene; // forward declaration — no include needed in header

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
        void ShowPerformanceWindow(bool* pOpen = nullptr);

        // ── Scene-aware panels ────────────────────────────────────────────
        // selectedIndex: index of selected model (-1 = none)
        void ShowSceneHierarchyWindow(Scene& scene, int& selectedIndex,
                                      bool* pOpen = nullptr);
        void ShowPropertiesWindow(Scene& scene, int selectedIndex,
                                  bool* pOpen = nullptr);

    private:
        void SetupStyle();
    };

} // namespace Atometa