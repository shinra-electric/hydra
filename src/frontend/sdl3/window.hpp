#pragma once

#include "core/emulation_context.hpp"
#include "frontend/sdl3/const.hpp"

namespace hydra::frontend::sdl3 {

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
    horizon::hid::NpadButtons buttons{horizon::hid::NpadButtons::None};
    u32 finger_id{invalid<u32>()};

    void UpdateWindowTitle();
    void SetWindowTitle(const std::string& title);
};

} // namespace hydra::frontend::sdl3
