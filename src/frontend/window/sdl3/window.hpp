#pragma once

#include "frontend/window/sdl3/const.hpp"
#include "frontend/window/window_base.hpp"

namespace Hydra::Frontend::Window::SDL3 {

class Window : public WindowBase {
  public:
    Window();
    ~Window() override;

    void Run() override;

  private:
    SDL_Window* window;
};

} // namespace Hydra::Frontend::Window::SDL3
