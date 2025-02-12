#pragma once

#include "horizon/kernel.hpp"

namespace Hydra::HW::MMU {
class MMUBase;
}

namespace Hydra::HW::Display {
class DisplayBase;
}

namespace Hydra::Horizon {

class OS {
  public:
    OS();
    ~OS();

    void SetDisplay(HW::Display::DisplayBase* display, u32 display_id);

    void SetMMU(HW::MMU::MMUBase* mmu);

    // Getters
    Kernel& GetKernel() { return kernel; }

  private:
    Kernel kernel;
};

} // namespace Hydra::Horizon
