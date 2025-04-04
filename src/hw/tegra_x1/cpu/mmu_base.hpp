#pragma once

#include "horizon/const.hpp"
#include "hw/tegra_x1/cpu/memory_base.hpp"

namespace Hydra::HW::TegraX1::CPU {

class MMUBase {
  public:
    static MMUBase& GetInstance();

    MMUBase();
    virtual ~MMUBase();

    virtual MemoryBase* AllocateMemory(usize size) = 0;
    virtual void FreeMemory(MemoryBase* memory) = 0;
    virtual uptr GetMemoryPtr(MemoryBase* memory) const = 0;

    virtual void Map(vaddr va, usize size, MemoryBase* memory,
                     const Horizon::MemoryState state) = 0;
    void Map(vaddr va, MemoryBase* memory, const Horizon::MemoryState state) {
        Map(va, memory->GetSize(), memory, state);
    }
    virtual void Map(vaddr dst_va, vaddr src_va, usize size) = 0;
    virtual void Unmap(vaddr va, usize size) = 0;
    virtual void ResizeHeap(vaddr va, usize size) = 0;

    virtual uptr UnmapAddr(vaddr va) const = 0;
    virtual Horizon::MemoryInfo QueryMemory(vaddr va) const = 0;

    template <typename T> T Load(vaddr va) const {
        return *reinterpret_cast<T*>(UnmapAddr(va));
    }

    template <typename T> void Store(vaddr va, T value) const {
        *reinterpret_cast<T*>(UnmapAddr(va)) = value;
    }
};

} // namespace Hydra::HW::TegraX1::CPU
