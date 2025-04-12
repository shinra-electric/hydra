#pragma once

#include "core/hw/tegra_x1/cpu/memory_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Dynarmic {

class Memory : public MemoryBase {
  public:
    Memory(usize size) : MemoryBase(size) {
        ptr = reinterpret_cast<uptr>(malloc(size));
    }
    ~Memory() { free(reinterpret_cast<void*>(ptr)); }

    void Resize(usize new_size) {
        free(reinterpret_cast<void*>(ptr));
        ptr = reinterpret_cast<uptr>(malloc(new_size));
    }

    // Getters
    uptr GetPtr() const { return ptr; }

  private:
    uptr ptr;
};

} // namespace Hydra::HW::TegraX1::CPU::Dynarmic
