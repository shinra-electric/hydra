#pragma once

#include "hw/tegra_x1/cpu/hypervisor/page_table.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU();

    uptr AllocateAndMap(vaddr va, usize size) override;
    void UnmapAndFree(vaddr va, usize size) override;

    uptr UnmapAddr(vaddr va) const override;

  private:
    // Page table
    PageTable page_table;

    // TODO: use a proper allocator
    uptr physical_memory_ptr;
    u64 physical_memory_cur{0};
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
