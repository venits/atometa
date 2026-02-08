#include "Atometa/Core/Application.h"
#include "Atometa/Core/Logger.h"
#include "Atometa/Core/Input.h"
#include "Atometa/Renderer/Renderer.h"
#include "Atometa/Renderer/Shader.h"
#include "Atometa/Renderer/Camera.h"
#include "Atometa/Renderer/Mesh.h"
#include "Atometa/UI/ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Atometa {
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name) {
        ATOMETA_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        Logger::Init();
        ATOMETA_INFO("========================================");
        ATOMETA_INFO("  Atometa Engine v0.1.0");
        ATOMETA_INFO("  3D Chemistry Simulation");
        ATOMETA_INFO("========================================");

        WindowProperties props;
        props.Title = name;
        m_Window = CreateScope<Window>(props);

        // Initialize renderer
        Renderer::Init();
        
        // Initialize ImGui
        m_ImGuiLayer = CreateScope<ImGuiLayer>();
        m_ImGuiLayer->OnAttach(m_Window->GetNativeWindow());
    }

    Application::~Application() {
        m_ImGuiLayer->OnDetach();
        Renderer::Shutdown();
        Logger::Shutdown();
    }

    void Application::Run() {
        ATOMETA_INFO("Application started");

        // Create shader
        Shader shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

        // Create camera
        Camera camera(45.0f, m_Window->GetAspectRatio());

        // Create 3D models
        Mesh sphereMesh = Mesh::CreateSphere(1.0f, 32, 16);
        Mesh cubeMesh = Mesh::CreateCube(1.0f);

        // Simple camera controls state
        static bool firstMouse = true;
        static float lastX = 0.0f;
        static float lastY = 0.0f;

        // UI state
        bool showDemo = false;
        bool showViewport = true;
        bool showProperties = true;
        bool showHierarchy = true;
        bool showPerformance = true;

        while (m_Running && !m_Window->ShouldClose()) {
            float time = (float)glfwGetTime();
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            // Camera controls
            if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
                glm::vec2 mousePos = Input::GetMousePosition();
                
                if (firstMouse) {
                    lastX = mousePos.x;
                    lastY = mousePos.y;
                    firstMouse = false;
                }

                float deltaX = mousePos.x - lastX;
                float deltaY = lastY - mousePos.y;
                
                lastX = mousePos.x;
                lastY = mousePos.y;

                camera.Rotate(deltaX, deltaY);
            } else {
                firstMouse = true;
            }

            // Clear screen
            Renderer::Clear(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));

            // Render 3D scene
            shader.Bind();
            
            glm::mat4 viewProjection = camera.GetProjectionMatrix() * camera.GetViewMatrix();
            shader.SetMat4("u_ViewProjection", viewProjection);
            shader.SetVec3("u_LightPos", glm::vec3(5.0f, 5.0f, 5.0f));
            shader.SetVec3("u_ViewPos", camera.GetPosition());
            
            // Render sphere (atom)
            glm::mat4 model = glm::mat4(1.0f);
            shader.SetMat4("u_Model", model);
            shader.SetVec3("u_Color", glm::vec3(1.0f, 0.3f, 0.3f));
            sphereMesh.Draw();
            
            // Render cube (for reference)
            model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f));
            shader.SetMat4("u_Model", model);
            shader.SetVec3("u_Color", glm::vec3(0.3f, 0.8f, 0.3f));
            cubeMesh.Draw();
            
            shader.Unbind();

            // ImGui UI
            m_ImGuiLayer->Begin();
            
            // Main menu bar
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("New", "Ctrl+N")) { }
                    if (ImGui::MenuItem("Open", "Ctrl+O")) { }
                    if (ImGui::MenuItem("Save", "Ctrl+S")) { }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Exit", "Alt+F4")) { Close(); }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit")) {
                    if (ImGui::MenuItem("Undo", "Ctrl+Z")) { }
                    if (ImGui::MenuItem("Redo", "Ctrl+Y")) { }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View")) {
                    ImGui::MenuItem("Viewport", nullptr, &showViewport);
                    ImGui::MenuItem("Properties", nullptr, &showProperties);
                    ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
                    ImGui::MenuItem("Performance", nullptr, &showPerformance);
                    ImGui::Separator();
                    ImGui::MenuItem("Demo Window", nullptr, &showDemo);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Simulate")) {
                    if (ImGui::MenuItem("Play", "Space")) { }
                    if (ImGui::MenuItem("Pause")) { }
                    if (ImGui::MenuItem("Reset")) { }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help")) {
                    if (ImGui::MenuItem("Documentation")) { }
                    if (ImGui::MenuItem("About")) { }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            
            // Show windows
            if (showDemo) m_ImGuiLayer->ShowDemoWindow();
            if (showViewport) m_ImGuiLayer->ShowViewportWindow(&showViewport);
            if (showProperties) m_ImGuiLayer->ShowPropertiesWindow(&showProperties);
            if (showHierarchy) m_ImGuiLayer->ShowSceneHierarchyWindow(&showHierarchy);
            if (showPerformance) m_ImGuiLayer->ShowPerformanceWindow(&showPerformance);
            
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }

        ATOMETA_INFO("Application shutdown");
    }

    void Application::Close() {
        m_Running = false;
    }
}