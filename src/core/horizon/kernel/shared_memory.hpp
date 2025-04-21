#pragma once

#include "core/horizon/kernel/const.hpp"
#include "core/hw/tegra_x1/cpu/memory_base.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
} // namespace Hydra::HW::TegraX1::CPU

namespace Hydra::Horizon::Kernel {

class SharedMemory : public Handle {
  public:
    SharedMemory(usize size);
    ~SharedMemory() override;

    void MapToRange(const range<uptr> range_, MemoryPermission perm);

    // Getters
    uptr GetPtr() const;

  private:
    HW::TegraX1::CPU::MemoryBase* memory;
};

} // namespace Hydra::Horizon::Kernel
