#include "Atometa/Core/Application.h"
#include "Atometa/Core/Logger.h"

class AtometaApp : public Atometa::Application {
public:
    AtometaApp() : Application("Atometa") {
        ATOMETA_INFO("AtometaApp initialized");
    }

    ~AtometaApp() {
        ATOMETA_INFO("AtometaApp destroyed");
    }
};

Atometa::Application* Atometa::CreateApplication() {
    return new AtometaApp();
}

int main(int argc, char** argv) {
    auto app = Atometa::CreateApplication();
    app->Run();
    delete app;
    return 0;
}