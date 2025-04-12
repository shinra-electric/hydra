#pragma once

#include "frontend/sdl3/const.hpp"
#include "frontend/window_base.hpp"

namespace Hydra::Frontend::SDL3 {

class Window : public WindowBase {
  public:
    Window(int argc, const char* argv[]);
    ~Window();

    void Run();

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    // Inputs
    Horizon::HID::NpadButtons buttons = Horizon::HID::NpadButtons::None;
};

} // namespace Hydra::Frontend::SDL3
