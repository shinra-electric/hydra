#pragma once

#include "core/hw/tegra_x1/cpu/memory_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

class Memory : public MemoryBase {
  public:
    Memory(paddr_t base_pa_, usize size) : MemoryBase(size), base_pa{base_pa_} {}

    // Getters
    paddr_t GetBasePa() const { return base_pa; }

  private:
    paddr_t base_pa;
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
