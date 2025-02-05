#pragma once

#include "common.hpp"

namespace Hydra::Horizon {

class OS;

}

namespace Hydra::HW::MMU::Hypervisor {

class MMU;

}

namespace Hydra::HW::CPU::Hypervisor {

class CPU;

}

namespace Hydra::Hypervisor {

class Hypervisor {
  public:
    Hypervisor(Horizon::OS& horizon_);
    ~Hypervisor();

    void LoadROM(Rom* rom);

    void Run();

  private:
    Horizon::OS& horizon;

    HW::MMU::Hypervisor::MMU* mmu;
    HW::CPU::Hypervisor::CPU* cpu;
};

} // namespace Hydra::Hypervisor
