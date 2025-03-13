#include "frontend/window/sdl3/window.hpp"

int main(int argc, const char* argv[]) {
    // Window
    // TODO: choose based on frontend
    Hydra::Frontend::Window::WindowBase* window;
    {
        window = new Hydra::Frontend::Window::SDL3::Window(argc, argv);
    }

    window->Run();

    return 0;
}
