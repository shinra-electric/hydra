#include "frontend/sdl3/window.hpp"

namespace Hydra::Frontend::SDL3 {

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

    if (!SDL_CreateWindowAndRenderer(APP_NAME, 1280, 720, 0, &window,
                                     &renderer)) {
        LOG_FATAL(SDL3Window, "Failed to create SDL3 window/renderer: {}",
                  SDL_GetError());
        return;
    }

    // Begin emulation
    emulation_context.SetSurface(SDL_GetRenderMetalLayer(renderer));
    emulation_context.LoadRom(rom_filename);
    emulation_context.Run();

    // Configure input
    Horizon::OS::GetInstance().GetInputManager().ConnectNpad(
        Horizon::HID::NpadIdType::Handheld,
        Horizon::HID::NpadStyleSet::Handheld,
        Horizon::HID::NpadAttributes::IsConnected);
}

Window::~Window() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::Run() {
    bool running = true;
    while (running) {
        auto& input_manager = Horizon::OS::GetInstance().GetInputManager();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

#define KEY_CASES                                                              \
    KEY_CASE(RETURN, Plus);                                                    \
    KEY_CASE(TAB, Minus);                                                      \
    KEY_CASE(W, Up);                                                           \
    KEY_CASE(A, Left);                                                         \
    KEY_CASE(S, Down);                                                         \
    KEY_CASE(D, Right);                                                        \
    KEY_CASE(I, X);                                                            \
    KEY_CASE(J, Y);                                                            \
    KEY_CASE(K, B);                                                            \
    KEY_CASE(L, A);

#define KEY_CASE(sdl_key, button)                                              \
    case SDLK_##sdl_key:                                                       \
        buttons |= Horizon::HID::NpadButtons::button;                          \
        break;

            // Key down
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    KEY_CASES;
                default:
                    break;
                }
            }

#undef KEY_CASE

#define KEY_CASE(sdl_key, button)                                              \
    case SDLK_##sdl_key:                                                       \
        buttons &= ~Horizon::HID::NpadButtons::button;                         \
        break;

            // Key up
            else if (event.type == SDL_EVENT_KEY_UP) {
                switch (event.key.key) {
                    KEY_CASES;
                default:
                    break;
                }
            }

#undef KEY_CASE

            // Mouse down
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    finger_id = input_manager.BeginTouch();
                    break;
                default:
                    break;
                }
            }

            // Mouse up
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    input_manager.EndTouch(finger_id);
                    finger_id = invalid<u32>();
                    break;
                default:
                    break;
                }
            }

#undef KEY_CASE

            // Mouse move
        }

#undef KEY_CASES

        if (emulation_context.IsRunning()) {
            // Input

            // Npad
            input_manager.SetNpadButtons(Horizon::HID::NpadIdType::Handheld,
                                         buttons);

            // Touch
            input_manager.UpdateTouchStates();
            if (finger_id != invalid<u32>()) {
                f32 x, y;
                SDL_GetMouseState(&x, &y);
                input_manager.SetTouchState({
                    .finger_id = finger_id,
                    .x = static_cast<u32>(x),
                    .y = static_cast<u32>(y),
                    // TODO: other stuff
                });
            }

            emulation_context.Present();
        }
    }
}

} // namespace Hydra::Frontend::SDL3
