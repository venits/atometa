#include "Atometa/UI/ImGuiLayer.h"
#include "Atometa/Core/Logger.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace Atometa {

    ImGuiLayer::ImGuiLayer() {
    }

    ImGuiLayer::~ImGuiLayer() {
    }

    void ImGuiLayer::OnAttach(GLFWwindow* window) {
        ATOMETA_INFO("Initializing ImGui...");

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        
        // Enable docking and viewports
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Setup style
        SetupStyle();

        // When viewports are enabled, tweak WindowRounding
        // ImGuiStyle& style = ImGui::GetStyle();
        // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        //     style.WindowRounding = 0.0f;
        //     style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        // }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        ATOMETA_INFO("ImGui initialized successfully");
    }

    void ImGuiLayer::OnDetach() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::Begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End() {
        ImGuiIO& io = ImGui::GetIO();
        
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and render additional platform windows
        // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        //     GLFWwindow* backup_current_context = glfwGetCurrentContext();
        //     ImGui::UpdatePlatformWindows();
        //     ImGui::RenderPlatformWindowsDefault();
        //     glfwMakeContextCurrent(backup_current_context);
        // }
    }

    void ImGuiLayer::ShowDemoWindow() {
        ImGui::ShowDemoWindow();
    }

    void ImGuiLayer::ShowViewportWindow(bool* pOpen) {
        ImGui::Begin("3D Viewport", pOpen, ImGuiWindowFlags_NoScrollbar);
        
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        
        ImGui::Text("Viewport Size: %.0fx%.0f", viewportSize.x, viewportSize.y);
        
        // Here we'll render the 3D scene to a framebuffer
        // For now, just show a placeholder
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "3D Scene Renders Here");
        
        ImGui::End();
    }

    void ImGuiLayer::ShowPropertiesWindow(bool* pOpen) {
        ImGui::Begin("Properties", pOpen);
        
        ImGui::SeparatorText("Selected Object");
        
        static char name[128] = "Carbon Atom";
        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
        
        ImGui::SeparatorText("Transform");
        
        static float position[3] = { 0.0f, 0.0f, 0.0f };
        ImGui::DragFloat3("Position", position, 0.1f);
        
        static float rotation[3] = { 0.0f, 0.0f, 0.0f };
        ImGui::DragFloat3("Rotation", rotation, 1.0f);
        
        static float scale[3] = { 1.0f, 1.0f, 1.0f };
        ImGui::DragFloat3("Scale", scale, 0.1f);
        
        ImGui::SeparatorText("Atom Properties");
        
        static const char* atomTypes[] = { "Hydrogen", "Carbon", "Nitrogen", "Oxygen" };
        static int currentAtom = 1;  // Carbon
        ImGui::Combo("Atom Type", &currentAtom, atomTypes, IM_ARRAYSIZE(atomTypes));
        
        static float mass = 12.011f;
        ImGui::InputFloat("Mass (u)", &mass, 0.001f, 1.0f, "%.3f");
        
        static float radius = 0.70f;
        ImGui::SliderFloat("Radius (Å)", &radius, 0.1f, 2.0f);
        
        static float color[3] = { 0.5f, 0.5f, 0.5f };
        ImGui::ColorEdit3("Color", color);
        
        ImGui::End();
    }

    void ImGuiLayer::ShowSceneHierarchyWindow(bool* pOpen) {
        ImGui::Begin("Scene Hierarchy", pOpen);
        
        ImGui::SeparatorText("Objects");
        
        if (ImGui::TreeNode("Water Molecule (H2O)")) {
            if (ImGui::Selectable("Oxygen", false))
                ATOMETA_INFO("Selected: Oxygen");
            if (ImGui::Selectable("Hydrogen 1", false))
                ATOMETA_INFO("Selected: Hydrogen 1");
            if (ImGui::Selectable("Hydrogen 2", false))
                ATOMETA_INFO("Selected: Hydrogen 2");
            ImGui::TreePop();
        }
        
        if (ImGui::TreeNode("Methane (CH4)")) {
            if (ImGui::Selectable("Carbon", false))
                ATOMETA_INFO("Selected: Carbon");
            if (ImGui::Selectable("Hydrogen 1", false))
                ATOMETA_INFO("Selected: H1");
            if (ImGui::Selectable("Hydrogen 2", false))
                ATOMETA_INFO("Selected: H2");
            if (ImGui::Selectable("Hydrogen 3", false))
                ATOMETA_INFO("Selected: H3");
            if (ImGui::Selectable("Hydrogen 4", false))
                ATOMETA_INFO("Selected: H4");
            ImGui::TreePop();
        }
        
        ImGui::Separator();
        
        if (ImGui::Button("Add Atom", ImVec2(-1, 0))) {
            ATOMETA_INFO("Add Atom clicked");
        }
        
        if (ImGui::Button("Add Molecule", ImVec2(-1, 0))) {
            ATOMETA_INFO("Add Molecule clicked");
        }
        
        ImGui::End();
    }

    void ImGuiLayer::ShowPerformanceWindow(bool* pOpen) {
        ImGui::Begin("Performance", pOpen);
        
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                    1000.0f / ImGui::GetIO().Framerate, 
                    ImGui::GetIO().Framerate);
        
        ImGui::Separator();
        
        ImGui::Text("Rendering Statistics:");
        ImGui::BulletText("Atoms: 0");
        ImGui::BulletText("Bonds: 0");
        ImGui::BulletText("Triangles: 0");
        ImGui::BulletText("Draw Calls: 0");
        
        ImGui::Separator();
        
        ImGui::Text("Memory Usage:");
        ImGui::BulletText("GPU Memory: N/A");
        ImGui::BulletText("CPU Memory: N/A");
        
        ImGui::End();
    }

    void ImGuiLayer::SetupStyle() {
        // Modern dark style
        ImGui::StyleColorsDark();
        
        ImGuiStyle& style = ImGui::GetStyle();
        
        // Rounding
        style.WindowRounding = 5.0f;
        style.FrameRounding = 4.0f;
        style.GrabRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        
        // Spacing
        style.WindowPadding = ImVec2(10, 10);
        style.FramePadding = ImVec2(8, 4);
        style.ItemSpacing = ImVec2(8, 6);
        style.ItemInnerSpacing = ImVec2(6, 4);
        
        // Colors - Professional dark theme
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        
        colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        
        colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        
        colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        
        colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        
        colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        
        colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        
        colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        
        // colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        // colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

}