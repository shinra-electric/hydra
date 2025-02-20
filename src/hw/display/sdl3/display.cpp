#include "hw/display/sdl3/display.hpp"

#include "hw/display/sdl3/layer.hpp"

namespace Hydra::HW::Display::SDL3 {

Display::Display() {
    if (!SDL_Init(SDL_INIT_VIDEO))
        LOG_ERROR(SDL3Display, "Failed to initialize SDL3: {}", SDL_GetError());

    window = SDL_CreateWindow("Hydra", 1280, 720, 0);
    if (!window)
        LOG_ERROR(SDL3Display, "Failed to create SDL3 window: {}",
                  SDL_GetError());
}

Display::~Display() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::Run() {
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
    }
}

LayerBase* Display::CreateLayerImpl(u32 binder_id) {
    return new Layer(binder_id);
}

} // namespace Hydra::HW::Display::SDL3
