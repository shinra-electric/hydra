#pragma once

#include "hw/mmu/mmu.hpp"

namespace Hydra::HW::MMU::Hypervisor {

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU();

    void MapMemory(Memory* mem) override;
    void UnmapMemory(Memory* mem) override;
    void ReprotectMemory(Memory* mem) override;
};

} // namespace Hydra::HW::MMU::Hypervisor
