#pragma once

#include "hw/tegra_x1/cpu/const.hpp"

namespace Hydra::HW::TegraX1::CPU {

class MMUBase {
  public:
    // TODO: let the implementation choose and return the virtual base
    virtual uptr AllocateAndMap(vaddr va, usize size) = 0;
    virtual void UnmapAndFree(vaddr va, usize size) = 0;
    virtual void ResizeHeap(vaddr va, usize size) = 0;

    virtual void Map(vaddr dst_va, vaddr src_va, usize size) = 0;
    virtual void Unmap(vaddr va, usize size) = 0;

    virtual uptr UnmapAddr(vaddr va) const = 0;

    template <typename T> T Load(vaddr va) const {
        return *reinterpret_cast<T*>(UnmapAddr(va));
    }

    template <typename T> void Store(vaddr va, T value) const {
        *reinterpret_cast<T*>(UnmapAddr(va)) = value;
    }
};

} // namespace Hydra::HW::TegraX1::CPU
