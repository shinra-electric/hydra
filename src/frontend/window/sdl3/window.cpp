#include "frontend/window/sdl3/window.hpp"

namespace Hydra::Frontend::Window::SDL3 {

Window::Window(int argc, const char* argv[]) {
    // Parse arguments
    // TODO: use a parser library
    ASSERT(argc == 2, SDL3Window, "Expected 1 argument, got {}", argc - 1);
    const char* rom_filename = argv[1];

    // SLD3 initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LOG_ERROR(SDL3Window, "Failed to initialize SDL3: {}", SDL_GetError());
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    if (!SDL_CreateWindowAndRenderer("Hydra", 1280, 720, 0, &window,
                                     &renderer)) {
        LOG_ERROR(SDL3Window, "Failed to create SDL3 window/renderer: {}",
                  SDL_GetError());
        return;
    }

    // Begin emulation
    InitializeEmulationContext(rom_filename);
}

Window::~Window() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::Run() {
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

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
