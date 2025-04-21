#pragma once

#include "core/emulation_context.hpp"
#include "frontend/sdl3/const.hpp"

namespace Hydra::Frontend::SDL3 {

class Window {
  public:
    Window(int argc, const char* argv[]);
    ~Window();

    void Run();

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    EmulationContext emulation_context;

    // Inputs
    Horizon::HID::NpadButtons buttons{Horizon::HID::NpadButtons::None};
    u32 finger_id{invalid<u32>()};
};

} // namespace Hydra::Frontend::SDL3
