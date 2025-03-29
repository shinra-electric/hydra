#pragma once

#include "horizon/const.hpp"

namespace Hydra::HW::TegraX1::CPU {
class Memory;
class MMUBase;
} // namespace Hydra::HW::TegraX1::CPU

namespace Hydra::Horizon {

class SharedMemory {
  public:
    SharedMemory(usize size);
    ~SharedMemory();

    void MapToRange(HW::TegraX1::CPU::MMUBase* mmu, const range<uptr> range);

    // Getters
    uptr GetPtr() const;

  private:
    HW::TegraX1::CPU::Memory* mem;
};

} // namespace Hydra::Horizon
