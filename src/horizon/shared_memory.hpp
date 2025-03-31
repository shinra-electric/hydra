#pragma once

#include "horizon/const.hpp"
#include "hw/tegra_x1/cpu/memory_base.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
} // namespace Hydra::HW::TegraX1::CPU

namespace Hydra::Horizon {

class SharedMemory {
  public:
    SharedMemory(usize size);
    ~SharedMemory();

    void MapToRange(const range<uptr> range_, MemoryPermission perm);

    // Getters
    uptr GetPtr() const;

  private:
    HW::TegraX1::CPU::MemoryBase* memory;
};

} // namespace Hydra::Horizon
