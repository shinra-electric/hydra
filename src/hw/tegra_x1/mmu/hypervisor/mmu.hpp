#pragma once

#include "hw/tegra_x1/mmu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::MMU::Hypervisor {

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU();

    void ReprotectMemory(Memory* mem) override;

  protected:
    void MapMemoryImpl(Memory* mem) override;
    void UnmapMemoryImpl(Memory* mem) override;
};

} // namespace Hydra::HW::TegraX1::MMU::Hypervisor
