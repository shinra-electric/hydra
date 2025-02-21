#include "frontend/window/sdl3/window.hpp"

namespace Hydra::Frontend::Window::SDL3 {

Window::Window() {
    if (!SDL_Init(SDL_INIT_VIDEO))
        LOG_ERROR(SDL3Display, "Failed to initialize SDL3: {}", SDL_GetError());

    window = SDL_CreateWindow("Hydra", 1280, 720, 0);
    if (!window)
        LOG_ERROR(SDL3Display, "Failed to create SDL3 window: {}",
                  SDL_GetError());
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
    }
}

} // namespace Hydra::Frontend::Window::SDL3
