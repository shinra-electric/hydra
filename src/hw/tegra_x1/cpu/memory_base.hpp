#pragma once

#include "hw/tegra_x1/cpu/const.hpp"

namespace Hydra::HW::TegraX1::CPU {

class MemoryBase {
  public:
    MemoryBase(usize size_) : size{size_} {}

    // Getters
    usize GetSize() const { return size; }

  private:
    usize size;
};

} // namespace Hydra::HW::TegraX1::CPU
