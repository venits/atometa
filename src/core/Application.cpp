#include "Atometa/Core/Application.h"
#include "Atometa/Core/Logger.h"
#include "Atometa/Core/Input.h"
#include "Atometa/Renderer/Renderer.h"
#include "Atometa/Renderer/Shader.h"
#include "Atometa/Renderer/Camera.h"
#include "Atometa/Scene/Scene.h"
#include "Atometa/UI/ImGuiLayer.h"
#include "Atometa/Chemistry/PeriodicTable.h"
#include "Atometa/Chemistry/MolecularFileIO.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>

namespace Atometa
{
    Application *Application::s_Instance = nullptr;

    Application::Application(const std::string &name)
    {
        ATOMETA_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        Logger::Init();
        ATOMETA_INFO("========================================");
        ATOMETA_INFO("  Atometa Engine v0.1.0");
        ATOMETA_INFO("  3D Chemistry Simulation");
        ATOMETA_INFO("========================================");

        PeriodicTable::Initialize();

        WindowProperties props;
        props.Title = name;
        props.IconPath = "assets/icons/app.ico";
        m_Window = CreateScope<Window>(props);

        Renderer::Init();

        m_ImGuiLayer = CreateScope<ImGuiLayer>();
        m_ImGuiLayer->OnAttach(m_Window->GetNativeWindow());

        m_Shader = CreateScope<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
        m_Camera = CreateScope<Camera>(45.0f, m_Window->GetAspectRatio());

        m_Scene = CreateScope<Scene>();
        m_Scene->ResetScene();
    }

    Application::~Application()
    {
        m_ImGuiLayer->OnDetach();
        Renderer::Shutdown();
        Logger::Shutdown();
    }

    Scene &Application::GetScene()
    {
        return *m_Scene;
    }

    void Application::Run()
    {
        ATOMETA_INFO("Application started");

        // Camera controls
        static bool firstLeftMouse = true;
        static float leftLastX = 0.0f, leftLastY = 0.0f;
        static bool firstRightMouse = true;
        static float rightLastX = 0.0f, rightLastY = 0.0f;
        static float panSpeedMultiplier = 1.0f;

        // UI state
        bool showDemo = false;
        bool showViewport = true;
        bool showProperties = true;
        bool showHierarchy = true;
        bool showPerformance = true;
        bool showMolecules = true;
        bool showSimulation = true;
        bool showEnergyPlot = false;

        // Energy history for plotting
        std::vector<float> kineticHistory;
        std::vector<float> potentialHistory;
        std::vector<float> totalHistory;

        while (m_Running && !m_Window->ShouldClose())
        {
            // ImGui wants to capture mouse input (e.g. hovering over UI elements)
            bool isMouseOverImGui = ImGui::GetIO().WantCaptureMouse;

            float time = (float)glfwGetTime();
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            // ========== INPUT HANDLING ==========

            // Keyboard shortcuts
            if (Input::IsKeyPressed(GLFW_KEY_SPACE))
            {
                auto &sim = m_Scene->GetSimulation();
                if (sim.IsRunning())
                    sim.Pause();
                else
                    sim.Play();
            }
            if (Input::IsKeyPressed(GLFW_KEY_R))
            {
                m_Scene->GetSimulation().Reset();
            }

            static float cameraSensitivity = 0.2f;

            // Mouse controls
            float scroll = Input::GetMouseScroll();
            if (scroll != 0.0f && Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL) && !isMouseOverImGui)
            {
                panSpeedMultiplier += scroll * 0.1f;
                panSpeedMultiplier = glm::clamp(panSpeedMultiplier, 0.1f, 10.0f);
            }
            else if (scroll != 0.0f && !isMouseOverImGui)
            {
                m_Camera->Zoom(scroll);
            }

            // Left-click: Pan
            if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && !isMouseOverImGui)
            {
                glm::vec2 mousePos = Input::GetMousePosition();
                if (firstRightMouse)
                {
                    rightLastX = mousePos.x;
                    rightLastY = mousePos.y;
                    firstRightMouse = false;
                }
                float deltaX = mousePos.x - rightLastX;
                float deltaY = mousePos.y - rightLastY;
                rightLastX = mousePos.x;
                rightLastY = mousePos.y;
                m_Camera->Pan(deltaX * cameraSensitivity, deltaY * cameraSensitivity);
            }
            else
            {
                firstRightMouse = true;
            }

            // Right-click: Rotate
            if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !isMouseOverImGui)
            {
                glm::vec2 mousePos = Input::GetMousePosition();
                if (firstLeftMouse)
                {
                    leftLastX = mousePos.x;
                    leftLastY = mousePos.y;
                    firstLeftMouse = false;
                }
                float deltaX = mousePos.x - leftLastX;
                float deltaY = leftLastY - mousePos.y;
                leftLastX = mousePos.x;
                leftLastY = mousePos.y;
                m_Camera->Rotate(deltaX, deltaY);
            }
            else
            {
                firstLeftMouse = true;
            }

            // ========== UPDATE & RENDER ==========

            m_Scene->Update(deltaTime);
            Renderer::Clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
            m_Scene->Render(*m_Shader, *m_Camera);

            // ========== UI ==========

            m_ImGuiLayer->Begin();

            // Main menu bar
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New", "Ctrl+N"))
                        m_Scene->ClearMolecules();
                    if (ImGui::MenuItem("Open XYZ...", "Ctrl+O"))
                    {
                        Molecule mol;
                        if (MolecularFileIO::LoadXYZ("assets/molecules/water.xyz", mol))
                            m_Scene->AddMolecule(mol);
                    }
                    if (ImGui::MenuItem("Save XYZ...", "Ctrl+S"))
                    {
                        if (m_Scene->GetMoleculeCount() > 0)
                            MolecularFileIO::SaveXYZ("output.xyz", m_Scene->GetMolecule(0));
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Exit", "Alt+F4"))
                        Close();
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    ImGui::MenuItem("Molecules", nullptr, &showMolecules);
                    ImGui::MenuItem("Simulation", nullptr, &showSimulation);
                    ImGui::MenuItem("Energy Plot", nullptr, &showEnergyPlot);
                    ImGui::MenuItem("Properties", nullptr, &showProperties);
                    ImGui::MenuItem("Performance", nullptr, &showPerformance);
                    ImGui::EndMenu();
                }

                ImGui::SliderFloat("Camera Sensitivity", &cameraSensitivity, 0.2f, 5.0f);

                if (ImGui::BeginMenu("Molecules"))
                {
                    if (ImGui::MenuItem("Water"))
                        m_Scene->AddMolecule(Molecule::CreateWater());
                    if (ImGui::MenuItem("Methane"))
                        m_Scene->AddMolecule(Molecule::CreateMethane());
                    if (ImGui::MenuItem("Ethanol"))
                        m_Scene->AddMolecule(Molecule::CreateEthanol());
                    if (ImGui::MenuItem("Benzene"))
                        m_Scene->AddMolecule(Molecule::CreateBenzene());
                    if (ImGui::MenuItem("CO2"))
                        m_Scene->AddMolecule(Molecule::CreateCO2());
                    if (ImGui::MenuItem("Ammonia"))
                        m_Scene->AddMolecule(Molecule::CreateAmmonia());
                    ImGui::Separator();
                    if (ImGui::MenuItem("Clear All"))
                        m_Scene->ClearMolecules();
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Simulation"))
                {
                    auto &sim = m_Scene->GetSimulation();
                    if (ImGui::MenuItem("Play", "Space", nullptr, !sim.IsRunning()))
                        sim.Play();
                    if (ImGui::MenuItem("Pause", nullptr, nullptr, sim.IsRunning()))
                        sim.Pause();
                    if (ImGui::MenuItem("Reset", "R"))
                        sim.Reset();
                    ImGui::Separator();
                    if (ImGui::MenuItem("Minimize Energy"))
                    {
                        sim.Pause();
                        sim.MinimizeEnergy(m_Scene->GetMolecules());
                    }
                    if (ImGui::MenuItem("Run 1000 steps"))
                        sim.RunMD(m_Scene->GetMolecules(), 1000);
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            // ========== SIMULATION WINDOW ==========

            if (showSimulation)
            {
                ImGui::Begin("Simulation", &showSimulation);

                auto &sim = m_Scene->GetSimulation();
                auto &stats = sim.GetStatistics();

                // Status
                ImGui::SeparatorText("Status");
                const char *modeText = sim.IsRunning() ? "Running (MD)" : "Paused";
                ImGui::Text("Mode: %s", modeText);
                ImGui::Text("Steps: %u", stats.StepCount);
                ImGui::Text("Time: %.3f ps", stats.SimulationTime);

                // Energetics
                ImGui::SeparatorText("Energetics");
                ImGui::Text("Kinetic:    %8.2f kJ/mol", stats.KineticEnergy);
                ImGui::Text("Potential:  %8.2f kJ/mol", stats.PotentialEnergy);
                ImGui::Text("Total:      %8.2f kJ/mol", stats.TotalEnergy);
                ImGui::Text("Temperature: %6.1f K", stats.Temperature);
                ImGui::Text("Collisions: %u", stats.CollisionCount);

                // Controls
                ImGui::SeparatorText("Controls");
                if (sim.IsRunning())
                {
                    if (ImGui::Button("Pause", ImVec2(-1, 0)))
                        sim.Pause();
                }
                else
                {
                    if (ImGui::Button("Play (MD)", ImVec2(-1, 0)))
                        sim.Play();
                }
                if (ImGui::Button("Reset", ImVec2(-1, 0)))
                    sim.Reset();
                if (ImGui::Button("Minimize Energy", ImVec2(-1, 0)))
                {
                    sim.Pause();
                    sim.MinimizeEnergy(m_Scene->GetMolecules());
                }

                // MD Parameters
                ImGui::SeparatorText("MD Parameters");

                static float timeStep = 0.001f;
                static float temperature = 300.0f;
                static float damping = 0.1f;
                static bool useThermostat = true;
                static int integrator = 0;

                if (ImGui::SliderFloat("Time Step (ps)", &timeStep, 0.0001f, 0.01f, "%.4f"))
                {
                    auto params = sim.GetMDParameters();
                    params.TimeStep = timeStep;
                    sim.SetMDParameters(params);
                }
                if (ImGui::SliderFloat("Temperature (K)", &temperature, 0.0f, 1000.0f))
                {
                    auto params = sim.GetMDParameters();
                    params.Temperature = temperature;
                    sim.SetMDParameters(params);
                }
                if (ImGui::SliderFloat("Damping", &damping, 0.0f, 1.0f))
                {
                    auto params = sim.GetMDParameters();
                    params.Damping = damping;
                    sim.SetMDParameters(params);
                }
                if (ImGui::Checkbox("Thermostat", &useThermostat))
                {
                    auto params = sim.GetMDParameters();
                    params.UseThermostat = useThermostat;
                    sim.SetMDParameters(params);
                }

                const char *integrators[] = {"Verlet", "Leap-Frog", "RK4"};
                if (ImGui::Combo("Integrator", &integrator, integrators, 3))
                {
                    auto params = sim.GetMDParameters();
                    params.Integrator = static_cast<IntegratorType>(integrator);
                    sim.SetMDParameters(params);
                }

                // Collisions
                ImGui::SeparatorText("Collisions");
                static bool enableCollisions = true;
                static float restitution = 0.8f;
                static float friction = 0.3f;

                if (ImGui::Checkbox("Enable", &enableCollisions))
                {
                    sim.SetEnableCollisions(enableCollisions);
                }
                if (ImGui::SliderFloat("Restitution", &restitution, 0.0f, 1.0f))
                {
                    auto params = sim.GetCollisionParameters();
                    params.Restitution = restitution;
                    sim.SetCollisionParameters(params);
                }
                if (ImGui::SliderFloat("Friction", &friction, 0.0f, 1.0f))
                {
                    auto params = sim.GetCollisionParameters();
                    params.Friction = friction;
                    sim.SetCollisionParameters(params);
                }

                ImGui::End();
            }

            // ========== ENERGY PLOT ==========

            if (showEnergyPlot)
            {
                ImGui::Begin("Energy Plot", &showEnergyPlot);

                auto &stats = m_Scene->GetSimulation().GetStatistics();

                kineticHistory.push_back(stats.KineticEnergy);
                potentialHistory.push_back(stats.PotentialEnergy);
                totalHistory.push_back(stats.TotalEnergy);

                if (kineticHistory.size() > 1000)
                {
                    kineticHistory.erase(kineticHistory.begin());
                    potentialHistory.erase(potentialHistory.begin());
                    totalHistory.erase(totalHistory.begin());
                }

                ImGui::PlotLines("Kinetic", kineticHistory.data(), kineticHistory.size(),
                                 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
                ImGui::PlotLines("Potential", potentialHistory.data(), potentialHistory.size(),
                                 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
                ImGui::PlotLines("Total", totalHistory.data(), totalHistory.size(),
                                 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));

                if (ImGui::Button("Clear"))
                {
                    kineticHistory.clear();
                    potentialHistory.clear();
                    totalHistory.clear();
                }

                ImGui::End();
            }

            // ========== MOLECULES WINDOW ==========

            if (showMolecules)
            {
                ImGui::Begin("Molecules", &showMolecules);
                ImGui::Text("Count: %zu", m_Scene->GetMoleculeCount());
                ImGui::Separator();

                for (size_t i = 0; i < m_Scene->GetMoleculeCount(); ++i)
                {
                    const auto &mol = m_Scene->GetMolecule(i);
                    if (ImGui::TreeNode((void *)(intptr_t)i, "%s", mol.GetName().c_str()))
                    {
                        ImGui::Text("Formula: %s", mol.GetFormula().c_str());
                        ImGui::Text("Weight: %.2f u", mol.GetMolecularWeight());
                        ImGui::Text("Atoms: %u", mol.GetAtomCount());
                        ImGui::Text("Bonds: %u", mol.GetBondCount());
                        if (ImGui::Button("Remove"))
                            m_Scene->RemoveMolecule(i);
                        ImGui::TreePop();
                    }
                }
                ImGui::End();
            }

            // Other windows
            if (showDemo)
                m_ImGuiLayer->ShowDemoWindow();
            if (showProperties)
                m_ImGuiLayer->ShowPropertiesWindow(&showProperties);
            if (showPerformance)
                m_ImGuiLayer->ShowPerformanceWindow(&showPerformance);

            m_ImGuiLayer->End();
            m_Window->OnUpdate();
        }

        ATOMETA_INFO("Application shutdown");
    }

    void Application::Close()
    {
        m_Running = false;
    }
}