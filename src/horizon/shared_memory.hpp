#pragma once

#include "horizon/const.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
} // namespace Hydra::HW::TegraX1::CPU

namespace Hydra::Horizon {

class SharedMemory {
  public:
    SharedMemory(usize size_) : size(size_) {}
    ~SharedMemory() = default;

    void MapToRange(HW::TegraX1::CPU::MMUBase* mmu, const range<uptr> range_);

    // Getters
    const range<uptr> GetRange() const { return range; }

  private:
    usize size;
    range<uptr> range;
};

} // namespace Hydra::Horizon
