#pragma once

#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU();

    void ReprotectMemory(Memory* mem) override;

  protected:
    void MapMemoryImpl(Memory* mem) override;
    void UnmapMemoryImpl(Memory* mem) override;
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
