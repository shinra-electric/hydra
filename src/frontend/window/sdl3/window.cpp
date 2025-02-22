#include "frontend/window/sdl3/window.hpp"

// HACK
const std::string rom_filename =
    "/Users/samuliak/Documents/deko3d_examples/build/0_hello_world.nro";

namespace Hydra::Frontend::Window::SDL3 {

Window::Window() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LOG_ERROR(SDL3Window, "Failed to initialize SDL3: {}", SDL_GetError());
        return;
    }

    if (!SDL_CreateWindowAndRenderer("Hydra", 1280, 720, 0, &window,
                                     &renderer)) {
        LOG_ERROR(SDL3Window, "Failed to create SDL3 window/renderer: {}",
                  SDL_GetError());
        return;
    }
}

Window::~Window() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::Run() {
    // HACK
    i32 timer = 60;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // HACK
        if (!IsEmulating() && timer-- <= 0) {
            InitializeEmulationContext(rom_filename);
        }

        // HACK
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        if (IsEmulating())
            Present();
    }
}

void* Window::CreateSurfaceImpl() {
    // TODO: choose based on Renderer backend
    {
        return SDL_GetRenderMetalLayer(renderer);
    }
}

} // namespace Hydra::Frontend::Window::SDL3
