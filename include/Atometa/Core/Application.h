#pragma once

#include "Core.h"
#include "Window.h"

namespace Atometa {

    class ImGuiLayer;
    class Scene;
    class Shader;
    class Camera;
    class NetworkLayer;

    class Application {
    public:
        explicit Application(const std::string& name = "Atometa");
        virtual ~Application();

        void Run();
        void Close();

        Window& GetWindow() { return *m_Window; }
        Scene&  GetScene();

        static Application& Get() { return *s_Instance; }

    private:
        void RenderUI(float& cameraSensitivity);
        void RenderSessionWindow();
        void BroadcastCamera();

    private:
        Scope<Window>       m_Window;
        Scope<ImGuiLayer>   m_ImGuiLayer;
        Scope<Scene>        m_Scene;
        Scope<Shader>       m_Shader;
        Scope<Camera>       m_Camera;
        Scope<NetworkLayer> m_Network;

        bool  m_Running         = true;
        bool  m_ShowSession     = true;
        bool  m_ShowPerformance = true;
        bool  m_ShowScene       = true;
        bool  m_ShowProperties  = true;
        int   m_SelectedModel   = -1;
        float m_LastFrameTime   = 0.0f;

        static Application* s_Instance;
    };

    Application* CreateApplication();

} // namespace Atometa