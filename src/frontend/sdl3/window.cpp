#include "frontend/sdl3/window.hpp"

namespace hydra::frontend::sdl3 {

Window::Window(int argc, const char* argv[]) {
    // Parse arguments
    // TODO: use a parser library
    ASSERT(argc == 2, SDL3Window, "Expected 1 argument, got {}", argc - 1);
    const char* rom_filename = argv[1];

    // SLD3 initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LOG_FATAL(SDL3Window, "Failed to initialize SDL3: {}", SDL_GetError());
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    if (!SDL_CreateWindowAndRenderer(APP_NAME, 1280, 720, SDL_WINDOW_RESIZABLE,
                                     &window, &renderer)) {
        LOG_FATAL(SDL3Window, "Failed to create SDL3 window/renderer: {}",
                  SDL_GetError());
        return;
    }

    // Begin emulation
    emulation_context.SetSurface(SDL_GetRenderMetalLayer(renderer));
    emulation_context.LoadRom(rom_filename);
    emulation_context.Run();
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

        if (emulation_context.IsRunning()) {
            // Present
            i32 width, height;
            SDL_GetWindowSize(window, &width, &height);
            bool dt_average_updated;
            emulation_context.ProgressFrame(width, height, dt_average_updated);

            // Update window title
            if (dt_average_updated)
                UpdateWindowTitle();
        }
    }
}

void Window::UpdateWindowTitle() {
    const auto dt = emulation_context.GetLastDeltaTimeAverage();
    std::string fps_str;
    if (dt == 0.0f)
        fps_str = "0";
    else
        fps_str = std::to_string((u32)std::round(1.0f / dt));

    // TODO: title name
    const auto title =
        fmt::format("Hydra | TODO(TITLE_NAME) - 0x{:016x} | {} | {} FPS",
                    emulation_context.GetTitleID(),
                    Config::GetInstance().GetGpuRenderer(), fps_str);
    SetWindowTitle(title);
}

void Window::SetWindowTitle(const std::string& title) {
    SDL_SetWindowTitle(window, title.c_str());
}

} // namespace hydra::frontend::sdl3
