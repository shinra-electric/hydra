#pragma once

#include "hw/tegra_x1/cpu/hypervisor/page_table.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

constexpr uptr KERNEL_REGION_BASE = 0xF0000000;
constexpr usize KERNEL_REGION_SIZE = 0x10000000;
constexpr usize KERNEL_MEM_SIZE = 0x1000;

constexpr uptr EXCEPTION_TRAMPOLINE_OFFSET = 0x800;

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU();

    uptr AllocateAndMap(vaddr va, usize size) override;
    void UnmapAndFree(vaddr va, usize size) override;
    void ResizeHeap(vaddr va, usize size) override;

    void Map(vaddr dst_va, vaddr src_va, usize size) override;
    void Unmap(vaddr va, usize size) override;

    uptr UnmapAddr(vaddr va) const override;

    // Getters
    const PageTable& GetUserPageTable() const { return user_page_table; }
    const PageTable& GetKernelPageTable() const { return kernel_page_table; }

  private:
    // Page table
    PageTable user_page_table;
    PageTable kernel_page_table;

    // TODO: use a proper allocator
    uptr physical_memory_ptr;
    u64 physical_memory_cur{0};
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
