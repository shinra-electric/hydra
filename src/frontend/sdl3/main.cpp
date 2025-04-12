#include "frontend/sdl3/window.hpp"

int main(int argc, const char* argv[]) {
    // Config
    Hydra::Config config;

    // Window
    Hydra::Frontend::SDL3::Window window(argc, argv);
    window.Run();

    return 0;
}
