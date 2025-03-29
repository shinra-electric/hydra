#pragma once

#include "hw/tegra_x1/cpu/hypervisor/const.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

struct Page {
    uptr ptr;
    paddr pa;
};

struct Allocation {
    uptr ptr;
    usize page_count;
};

constexpr usize PAGE_COUNT_ALIGNMENT = 4;

class PageAllocator {
  public:
    PageAllocator(paddr base_pa_, usize page_count);
    ~PageAllocator();

    Page GetNextPage();

  private:
    paddr base_pa;

    std::vector<Allocation> allocations;
    u32 current_page_in_allocation = 0;
    usize current_page = 0;

    void Allocate(usize page_count = PAGE_COUNT_ALIGNMENT);
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
