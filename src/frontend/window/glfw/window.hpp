#pragma once

#include "frontend/window/glfw/const.hpp"
#include "frontend/window/window_base.hpp"

namespace Hydra::Frontend::Window::GLFW {

class Window : public WindowBase {
  public:
    Window();
    ~Window() override;

    void Run() override;

  private:
    GLFWwindow* window;
};

} // namespace Hydra::Frontend::Window::GLFW
