#pragma once

#include "horizon/kernel.hpp"

namespace Hydra::HW::MMU {

class MMUBase;

} // namespace Hydra::HW::MMU

namespace Hydra::Horizon {

class OS {
  public:
    OS();
    ~OS();

    void SetMMU(HW::MMU::MMUBase* mmu);

    // Getters
    Kernel& GetKernel() { return kernel; }

  private:
    Kernel kernel;
};

} // namespace Hydra::Horizon
