#pragma once

#include "core/hw/tegra_x1/cpu/mmu.hpp"

namespace hydra::hw::tegra_x1::cpu::dynarmic {

struct MemoryRange {
    uptr ptr;
    horizon::kernel::MemoryState state{.type =
                                           horizon::kernel::MemoryType::Free};
};

// TODO: rework this
class Mmu : public IMmu {
  public:
    void Map(vaddr_t dst_va, uptr ptr, usize size,
             const horizon::kernel::MemoryState state) override;
    void Map(vaddr_t dst_va, vaddr_t src_va, usize size) override;
    void Unmap(vaddr_t va, usize size) override;

    void ResizeHeap(IMemory* heap_mem, vaddr_t va, usize size) override;

    uptr UnmapAddr(vaddr_t va) const override;
    MemoryRegion QueryRegion(vaddr_t va) const override;

  private:
    std::map<u64, MemoryRange> memory_ranges;
};

} // namespace hydra::hw::tegra_x1::cpu::dynarmic
