#pragma once

#include "hw/tegra_x1/cpu/memory_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

class Memory : public MemoryBase {
  public:
    Memory(paddr base_pa_, usize size) : MemoryBase(size), base_pa{base_pa_} {}

    // Getters
    paddr GetBasePa() const { return base_pa; }

  private:
    paddr base_pa;
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
