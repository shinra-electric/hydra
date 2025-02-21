#pragma once

#include "emulation_context.hpp"

namespace Hydra::Frontend::Window {

class WindowBase {
  public:
    virtual ~WindowBase() {
        if (emulation_context)
            delete emulation_context;
    }

    virtual void Run() = 0;

  protected:
    EmulationContext* emulation_context = nullptr;
};

} // namespace Hydra::Frontend::Window
