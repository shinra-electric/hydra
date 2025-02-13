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
    OS(HW::Bus& bus);
    ~OS();

    void SetMMU(HW::MMU::MMUBase* mmu);

    // Getters
    Kernel& GetKernel() { return kernel; }

  private:
    Kernel kernel;
};

} // namespace Hydra::Horizon
