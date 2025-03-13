#pragma once

#include "frontend/window/sdl3/const.hpp"
#include "frontend/window/window_base.hpp"

namespace Hydra::Frontend::Window::SDL3 {

class Window : public WindowBase {
  public:
    Window(int argc, const char* argv[]);
    ~Window() override;

    void Run() override;

  protected:
    void* CreateSurfaceImpl() override;

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

} // namespace Hydra::Frontend::Window::SDL3
