#include "Atometa/UI/ImGuiLayer.h"
#include "Atometa/Scene/Scene.h"
#include "Atometa/Core/Logger.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace Atometa {

    ImGuiLayer::ImGuiLayer() {}
    ImGuiLayer::~ImGuiLayer() {}

    void ImGuiLayer::OnAttach(GLFWwindow* window)
    {
        ATOMETA_INFO("Initializing ImGui...");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        SetupStyle();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        ATOMETA_INFO("ImGui initialized successfully");
    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiLayer::ShowDemoWindow()
    {
        ImGui::ShowDemoWindow();
    }

    void ImGuiLayer::ShowViewportWindow(bool* pOpen)
    {
        ImGui::Begin("3D Viewport", pOpen, ImGuiWindowFlags_NoScrollbar);
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Text("Viewport Size: %.0fx%.0f", viewportSize.x, viewportSize.y);
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "3D Scene Renders Here");
        ImGui::End();
    }

    // ── Scene Hierarchy ───────────────────────────────────────────────────

    void ImGuiLayer::ShowSceneHierarchyWindow(Scene& scene, int& selectedIndex,
                                               bool* pOpen)
    {
        ImGui::Begin("Scene", pOpen);

        // ── Header ────────────────────────────────────────────────────────
        ImGui::TextColored({0.4f, 0.8f, 1.0f, 1.f},
                           "Models (%zu)", scene.GetModelCount());
        ImGui::Separator();

        // ── Model list ────────────────────────────────────────────────────
        for (int i = 0; i < static_cast<int>(scene.GetModelCount()); ++i)
        {
            const auto& model = scene.GetModel(i);

            // Visibility toggle (eye icon via text)
            bool visible = model.IsVisible();
            ImGui::PushID(i);

            if (ImGui::Checkbox("##vis", &visible))
                scene.GetModel(i).SetVisible(visible);

            ImGui::SameLine();

            // Selectable row
            bool selected = (selectedIndex == i);
            char label[128];
            snprintf(label, sizeof(label), "%s  (%zu mesh%s)##model%d",
                     model.GetName().c_str(),
                     model.GetMeshCount(),
                     model.GetMeshCount() == 1 ? "" : "es",
                     i);

            if (ImGui::Selectable(label, selected,
                                  ImGuiSelectableFlags_SpanAllColumns))
                selectedIndex = selected ? -1 : i; // toggle

            // Right-click context menu
            if (ImGui::BeginPopupContextItem("model_ctx"))
            {
                if (ImGui::MenuItem("Remove"))
                {
                    scene.RemoveModel(i);
                    if (selectedIndex == i)  selectedIndex = -1;
                    else if (selectedIndex > i) selectedIndex--;
                    ImGui::EndPopup();
                    ImGui::PopID();
                    break; // list changed — restart next frame
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        // ── Empty scene hint ──────────────────────────────────────────────
        if (scene.GetModelCount() == 0)
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No models loaded.");
            ImGui::TextDisabled("Use Load Model below.");
        }

        // ── Load model ────────────────────────────────────────────────────
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        static char pathBuf[256] = "assets/models/";
        static char nameBuf[64]  = "";
        ImGui::SetNextItemWidth(-1.f);
        ImGui::InputText("##path", pathBuf, sizeof(pathBuf));
        ImGui::SetNextItemWidth(-1.f);
        ImGui::InputText("Display name##name", nameBuf, sizeof(nameBuf));

        if (ImGui::Button("Load Model", ImVec2(-1.f, 0.f)))
        {
            std::string path(pathBuf);
            std::string name(nameBuf);
            if (!path.empty())
            {
                int idx = scene.LoadModel(path, name.empty() ? path : name);
                if (idx >= 0)
                {
                    selectedIndex = idx;
                    nameBuf[0] = '\0';
                    ATOMETA_INFO("Loaded model: ", path);
                }
            }
        }

        // ── Clear all ─────────────────────────────────────────────────────
        if (scene.GetModelCount() > 0)
        {
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.6f,0.1f,0.1f,1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f,0.2f,0.2f,1.f));
            if (ImGui::Button("Clear Scene", ImVec2(-1.f, 0.f)))
            {
                scene.Clear();
                selectedIndex = -1;
            }
            ImGui::PopStyleColor(2);
        }

        ImGui::End();
    }

    // ── Properties ────────────────────────────────────────────────────────

    void ImGuiLayer::ShowPropertiesWindow(Scene& scene, int selectedIndex,
                                           bool* pOpen)
    {
        ImGui::Begin("Properties", pOpen);

        if (selectedIndex < 0 ||
            selectedIndex >= static_cast<int>(scene.GetModelCount()))
        {
            ImGui::TextDisabled("No model selected.");
            ImGui::End();
            return;
        }

        auto& model = scene.GetModel(selectedIndex);

        // ── Info ──────────────────────────────────────────────────────────
        ImGui::SeparatorText("Model Info");
        ImGui::Text("Name   : %s", model.GetName().c_str());
        ImGui::Text("Source : %s", model.GetSourcePath().c_str());
        ImGui::Text("Meshes : %zu", model.GetMeshCount());

        // ── Transform ─────────────────────────────────────────────────────
        ImGui::SeparatorText("Transform");

        glm::vec3 pos = model.GetPosition();
        if (ImGui::DragFloat3("Position", &pos.x, 0.01f))
            model.SetPosition(pos);

        glm::vec3 rot = model.GetRotation();
        if (ImGui::DragFloat3("Rotation", &rot.x, 0.5f, -360.f, 360.f))
            model.SetRotation(rot);

        float scale = model.GetScale();
        if (ImGui::DragFloat("Scale", &scale, 0.01f, 0.001f, 100.f))
            model.SetScale(scale);

        // ── Reset button ──────────────────────────────────────────────────
        ImGui::Spacing();
        if (ImGui::Button("Reset Transform", ImVec2(-1.f, 0.f)))
        {
            model.SetPosition({0.f, 0.f, 0.f});
            model.SetRotation({0.f, 0.f, 0.f});
            model.SetScale(1.f);
        }

        // ── Visibility ────────────────────────────────────────────────────
        ImGui::Spacing();
        ImGui::SeparatorText("Visibility");
        bool visible = model.IsVisible();
        if (ImGui::Checkbox("Visible", &visible))
            model.SetVisible(visible);

        ImGui::End();
    }

    // ── Performance ───────────────────────────────────────────────────────

    void ImGuiLayer::ShowPerformanceWindow(bool* pOpen)
    {
        ImGui::Begin("Performance", pOpen);

        ImGui::Text("%.3f ms/frame  (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::Separator();
        ImGui::Text("Memory:");
        ImGui::BulletText("GPU: N/A");
        ImGui::BulletText("CPU: N/A");

        ImGui::End();
    }

    // ── Style ─────────────────────────────────────────────────────────────

    void ImGuiLayer::SetupStyle()
    {
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowRounding   = 5.0f;
        style.FrameRounding    = 4.0f;
        style.GrabRounding     = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.WindowPadding    = ImVec2(10, 10);
        style.FramePadding     = ImVec2(8, 4);
        style.ItemSpacing      = ImVec2(8, 6);
        style.ItemInnerSpacing = ImVec2(6, 4);

        ImVec4* c = style.Colors;
        c[ImGuiCol_WindowBg]          = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        c[ImGuiCol_ChildBg]           = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        c[ImGuiCol_PopupBg]           = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        c[ImGuiCol_Border]            = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        c[ImGuiCol_FrameBg]           = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        c[ImGuiCol_FrameBgHovered]    = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        c[ImGuiCol_FrameBgActive]     = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        c[ImGuiCol_TitleBg]           = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        c[ImGuiCol_TitleBgActive]     = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        c[ImGuiCol_MenuBarBg]         = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        c[ImGuiCol_CheckMark]         = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        c[ImGuiCol_SliderGrab]        = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        c[ImGuiCol_SliderGrabActive]  = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        c[ImGuiCol_Button]            = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        c[ImGuiCol_ButtonHovered]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        c[ImGuiCol_ButtonActive]      = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        c[ImGuiCol_Header]            = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        c[ImGuiCol_HeaderHovered]     = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        c[ImGuiCol_HeaderActive]      = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        c[ImGuiCol_Tab]               = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        c[ImGuiCol_TabHovered]        = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        c[ImGuiCol_TabActive]         = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        c[ImGuiCol_DragDropTarget]    = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        c[ImGuiCol_TextSelectedBg]    = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        c[ImGuiCol_NavHighlight]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    }

} // namespace Atometa