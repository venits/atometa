#include "Atometa/Core/Application.h"
#include "Atometa/Core/Logger.h"
#include "Atometa/Core/Input.h"
#include "Atometa/Renderer/Renderer.h"
#include "Atometa/Renderer/Shader.h"
#include "Atometa/Renderer/Camera.h"
#include "Atometa/Scene/Scene.h"
#include "Atometa/UI/ImGuiLayer.h"
#include "Atometa/Network/NetworkLayer.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Atometa
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name)
    {
        ATOMETA_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        Logger::Init();
        ATOMETA_INFO("========================================");
        ATOMETA_INFO("  Atometa Engine v0.2.0");
        ATOMETA_INFO("  3D Medical Education Platform");
        ATOMETA_INFO("========================================");

        WindowProperties props;
        props.Title    = name;
        props.IconPath = "assets/icons/app.ico";

        m_Window = CreateScope<Window>(props);

        Renderer::Init();

        m_ImGuiLayer = CreateScope<ImGuiLayer>();
        m_ImGuiLayer->OnAttach(m_Window->GetNativeWindow());

        m_Shader = CreateScope<Shader>(
            "assets/shaders/basic.vert",
            "assets/shaders/basic.frag"
        );
        m_Camera = CreateScope<Camera>(45.0f, m_Window->GetAspectRatio());

        m_Scene   = CreateScope<Scene>();
        m_Network = CreateScope<NetworkLayer>();

        if (m_Scene->LoadModel("assets/models/heart.glb", "Heart") < 0)
            ATOMETA_WARN("Heart model not found — showing placeholder sphere");

        // Camera sync from host
        m_Network->SetOnMessage([this](const NetMessage& msg) {
            if (msg.Type == NetMsgType::CameraSync) {
                float yaw   = msg.Data.value("yaw",   0.f);
                float pitch = msg.Data.value("pitch", 0.f);
                float dist  = msg.Data.value("dist",  10.f);
                m_Camera->SetFromNetwork(yaw, pitch, dist);
            }
        });
    }

    Application::~Application()
    {
        m_ImGuiLayer->OnDetach();
        Renderer::Shutdown();
        Logger::Shutdown();
    }

    Scene& Application::GetScene()
    {
        return *m_Scene;
    }

    void Application::Run()
    {
        ATOMETA_INFO("Application started");

        bool  firstLeft  = true, firstRight = true;
        float leftLastX  = 0.f, leftLastY  = 0.f;
        float rightLastX = 0.f, rightLastY = 0.f;

        static float cameraSensitivity = 0.2f;

        while (m_Running && !m_Window->ShouldClose())
        {
            float time      = static_cast<float>(glfwGetTime());
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            bool overUI = ImGui::GetIO().WantCaptureMouse;

            // ── Input ─────────────────────────────────────────────────────
            float scroll = Input::GetMouseScroll();
            if (scroll != 0.f && !overUI)
                m_Camera->Zoom(scroll);

            if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !overUI)
            {
                glm::vec2 mp = Input::GetMousePosition();
                if (firstLeft) { leftLastX = mp.x; leftLastY = mp.y; firstLeft = false; }
                m_Camera->Rotate(
                    (mp.x - leftLastX) * cameraSensitivity,
                    (leftLastY - mp.y) * cameraSensitivity
                );
                leftLastX = mp.x; leftLastY = mp.y;
            }
            else { firstLeft = true; }

            if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && !overUI)
            {
                glm::vec2 mp = Input::GetMousePosition();
                if (firstRight) { rightLastX = mp.x; rightLastY = mp.y; firstRight = false; }
                m_Camera->Pan(
                    (mp.x - rightLastX) * cameraSensitivity,
                    (mp.y - rightLastY) * cameraSensitivity
                );
                rightLastX = mp.x; rightLastY = mp.y;
            }
            else { firstRight = true; }

            // ── Update & render ───────────────────────────────────────────
            m_Scene->Update(deltaTime);
            BroadcastCamera();

            Renderer::Clear(glm::vec4(0.08f, 0.08f, 0.10f, 1.0f));
            m_Scene->Render(*m_Shader, *m_Camera);

            // ── UI ────────────────────────────────────────────────────────
            m_ImGuiLayer->Begin();
            RenderUI(cameraSensitivity);
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }

        ATOMETA_INFO("Application shutdown");
    }

    void Application::BroadcastCamera()
    {
        if (m_Network->GetRole() != NetworkRole::Host) return;
        if (!m_Network->IsConnected())                 return;

        NetMessage msg;
        msg.Type          = NetMsgType::CameraSync;
        msg.Data["yaw"]   = m_Camera->GetYaw();
        msg.Data["pitch"] = m_Camera->GetPitch();
        msg.Data["dist"]  = m_Camera->GetDistance();
        m_Network->Send(msg);
    }

    void Application::RenderUI(float& cameraSensitivity)
    {
        // ── Menu bar ──────────────────────────────────────────────────────
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit", "Alt+F4")) Close();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Scene",       nullptr, &m_ShowScene);
                ImGui::MenuItem("Properties",  nullptr, &m_ShowProperties);
                ImGui::MenuItem("Session",     nullptr, &m_ShowSession);
                ImGui::MenuItem("Performance", nullptr, &m_ShowPerformance);
                ImGui::EndMenu();
            }
            ImGui::SetNextItemWidth(160.f);
            ImGui::SliderFloat("Sensitivity", &cameraSensitivity, 0.05f, 5.0f);
            ImGui::EndMainMenuBar();
        }

        if (m_ShowScene)
            m_ImGuiLayer->ShowSceneHierarchyWindow(*m_Scene, m_SelectedModel,
                                                   &m_ShowScene);
        if (m_ShowProperties)
            m_ImGuiLayer->ShowPropertiesWindow(*m_Scene, m_SelectedModel,
                                               &m_ShowProperties);
        if (m_ShowSession)
            RenderSessionWindow();
        if (m_ShowPerformance)
            m_ImGuiLayer->ShowPerformanceWindow(&m_ShowPerformance);
    }

    void Application::RenderSessionWindow()
    {
        ImGui::Begin("Session", &m_ShowSession, ImGuiWindowFlags_AlwaysAutoResize);

        auto role = m_Network->GetRole();

        if (role == NetworkRole::None)
        {
            ImGui::SeparatorText("Start a session");

            static uint16_t hostPort = 8080;
            ImGui::SetNextItemWidth(80.f);
            ImGui::InputScalar("Port##host", ImGuiDataType_U16, &hostPort);
            ImGui::SameLine();
            if (ImGui::Button("Host Session")) {
                if (m_Network->StartHost(hostPort))
                    ATOMETA_INFO("Hosting on port ", hostPort);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            static char joinIp[64]   = "127.0.0.1";
            static uint16_t joinPort = 8080;
            ImGui::SetNextItemWidth(140.f);
            ImGui::InputText("IP##join", joinIp, sizeof(joinIp));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80.f);
            ImGui::InputScalar("Port##join", ImGuiDataType_U16, &joinPort);
            ImGui::SameLine();
            if (ImGui::Button("Join Session")) {
                if (m_Network->Connect(joinIp, joinPort))
                    ATOMETA_INFO("Connecting to ", joinIp, ":", joinPort);
            }
        }
        else if (role == NetworkRole::Host)
        {
            ImGui::TextColored({0.2f,1.f,0.4f,1.f}, "● Hosting");
            ImGui::Text("Students connected: %u", m_Network->GetClientCount());
            if (ImGui::Button("Stop Session"))
                m_Network->StopHost();
        }
        else if (role == NetworkRole::Client)
        {
            if (m_Network->IsConnected())
                ImGui::TextColored({0.2f,1.f,0.4f,1.f}, "● Connected to session");
            else
                ImGui::TextColored({1.f,0.4f,0.2f,1.f}, "● Connecting...");
            if (ImGui::Button("Leave Session"))
                m_Network->Disconnect();
        }

        ImGui::End();
    }

    void Application::Close()
    {
        m_Running = false;
    }

} // namespace Atometa