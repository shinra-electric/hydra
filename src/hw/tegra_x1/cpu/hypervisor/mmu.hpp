#pragma once

#include "hw/tegra_x1/cpu/hypervisor/page_table.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU();

    void Allocate(MemoryAllocator& allocator) override {
        page_table.Allocate(allocator);
    }

    void MapPhysicalMemory(uptr ptr, uptr pa, usize size) override;
    void UnmapPhysicalMemory(uptr pa, usize size) override;

    void ReprotectMemory(Memory* mem) override;

    // Getters
    uptr GetPageTablePa() const { return page_table.GetMemory()->GetPa(); }
    // Memory* GetKernelRangeMemory() const { return kernel_range.mem; }

  protected:
    void MapMemoryImpl(Memory* mem) override;
    void UnmapMemoryImpl(Memory* mem) override;

  private:
    // Page table
    PageTable page_table;
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
