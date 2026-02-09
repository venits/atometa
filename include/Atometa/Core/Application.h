#pragma once

#include "Core.h"
#include "Window.h"

namespace Atometa {
    class ImGuiLayer;
    class Scene;
    class Shader;
    class Camera;

    class Application {
    public:
        Application(const std::string& name = "Atometa");
        virtual ~Application();

        void Run();
        void Close();

        Window& GetWindow() { return *m_Window; }
        Scene& GetScene();
        
        static Application& Get() { return *s_Instance; }

    private:
        Scope<Window> m_Window;
        Scope<ImGuiLayer> m_ImGuiLayer;
        Scope<Scene> m_Scene;
        Scope<Shader> m_Shader;
        Scope<Camera> m_Camera;
        
        bool m_Running = true;
        float m_LastFrameTime = 0.0f;
        static Application* s_Instance;
    };

    Application* CreateApplication();
}