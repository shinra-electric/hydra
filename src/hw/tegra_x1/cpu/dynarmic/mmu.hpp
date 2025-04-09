#pragma once

#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Dynarmic {

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU() override;

    MemoryBase* AllocateMemory(usize size) override;
    void FreeMemory(MemoryBase* memory) override;
    uptr GetMemoryPtr(MemoryBase* memory) const override;

    void Map(vaddr va, usize size, MemoryBase* memory,
             const Horizon::MemoryState state) override;
    void Map(vaddr dst_va, vaddr src_va, usize size) override;
    void Unmap(vaddr va, usize size) override;

    void ResizeHeap(vaddr va, usize size) override;

    uptr UnmapAddr(vaddr va) const override;
    Horizon::MemoryInfo QueryMemory(vaddr va) const override;

  private:
};

} // namespace Hydra::HW::TegraX1::CPU::Dynarmic
