#pragma once

#include "Core.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Atometa {
    class ImGuiLayer;

    class Application {
    public:
        Application(const std::string& name = "Atometa");
        virtual ~Application();

        void Run();
        void Close();

        Window& GetWindow() { return *m_Window; }
        static Application& Get() { return *s_Instance; }

    private:
        Scope<Window> m_Window;
        Scope<ImGuiLayer> m_ImGuiLayer;
        bool m_Running = true;
        float m_LastFrameTime = 0.0f;
        static Application* s_Instance;
    };

    Application* CreateApplication();
}