#include "frontend/window/glfw/window.hpp"

// HACK
const std::string rom_filename =
    "/Users/samuliak/Documents/deko3d_examples/build/0_hello_world.nro";

namespace Hydra::Frontend::Window::GLFW {

Window::Window() {
    glfwInit();

    window = glfwCreateWindow(1280, 720, "Hydra", nullptr, nullptr);
    if (!window) {
        // LOG_ERROR();
    }
}

Window::~Window() { glfwTerminate(); }

void Window::Run() {
    // emulation_context = new EmulationContext(rom_filename);

    // HACK
    i32 timer = 60;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // HACK
        if (!emulation_context && timer-- <= 0) {
            emulation_context = new EmulationContext(rom_filename);
        }

        // HACK
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        glfwSwapBuffers(window);
    }
}

} // namespace Hydra::Frontend::Window::GLFW
