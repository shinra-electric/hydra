#pragma once

#include "core/emulation_context.hpp"

namespace Hydra::Frontend {

class WindowBase {
  public:
    ~WindowBase() {
        if (emulation_context)
            delete emulation_context;
    }

  protected:
    void InitializeEmulationContext(const std::string& rom_filename,
                                    void* surface);

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

} // namespace Hydra::Frontend
