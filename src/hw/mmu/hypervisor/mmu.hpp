#pragma once

#include "hw/mmu/mmu.hpp"

namespace Hydra::HW::MMU::Hypervisor {

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU();

    void ReprotectMemory(Memory* mem) override;

  protected:
    void MapMemoryImpl(Memory* mem) override;
    void UnmapMemoryImpl(Memory* mem) override;
};

} // namespace Hydra::HW::MMU::Hypervisor
