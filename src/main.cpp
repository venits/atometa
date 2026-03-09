#include "Atometa/Core/Application.h"
#include "Atometa/Core/Logger.h"

// ── Entry point ───────────────────────────────────────────────────────────
// Windows GUI subsystem requires WinMain.
// All four parameters are unused at this level — the engine handles input.

#if defined(ATOMETA_PLATFORM_WINDOWS)

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <windows.h>

int WINAPI WinMain(
    _In_     HINSTANCE /*hInstance*/,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_     LPSTR     /*lpCmdLine*/,
    _In_     int       /*nCmdShow*/)
{
    auto* app = Atometa::CreateApplication();
    app->Run();
    delete app;
    return 0;
}

#else

// Linux / macOS — standard main
int main()
{
    auto* app = Atometa::CreateApplication();
    app->Run();
    delete app;
    return 0;
}

#endif

// ── App factory ───────────────────────────────────────────────────────────
class AtometaApp : public Atometa::Application {
public:
    AtometaApp() : Application("Atometa — Medical Education") {}
};

Atometa::Application* Atometa::CreateApplication()
{
    return new AtometaApp();
}