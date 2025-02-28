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
    void InitializeEmulationContext(const std::string& rom_filename);

    virtual void* CreateSurfaceImpl() = 0;

    void Present();

    bool IsEmulating() {
        if (emulation_context) {
            if (emulation_context->IsRunning())
                return true;
            else {
                delete emulation_context;
                emulation_context = nullptr;

                return false;
            }
        }

        return emulation_context != nullptr;
    }

  private:
    EmulationContext* emulation_context = nullptr;
};

} // namespace Hydra::Frontend::Window
