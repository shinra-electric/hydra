#include "core/hw/tegra_x1/cpu/mmu.hpp"

namespace hydra::hw::tegra_x1::cpu {

horizon::kernel::MemoryInfo IMmu::QueryMemory(vaddr_t va) const {
    horizon::kernel::MemoryInfo info;
    info.size = 0x0;

    // Resize to the left
    auto region = QueryRegion(va);
    do {
        // Resize
        info.addr = region.va;
        info.size = region.size;
        info.state = region.state;
        if (info.addr == 0x0)
            break;

        // Next
        region = QueryRegion(info.addr - 1);
    } while (region.state == info.state);

    // Resize to the right
    region = QueryRegion(info.addr + info.size);
    while (region.state == info.state) {
        // Resize
        info.size += region.size;

        // Next
        vaddr_t addr = info.addr + info.size;
        if (addr >= horizon::kernel::ADDRESS_SPACE.end)
            break;

        region = QueryRegion(addr);
    }

    // HACK
    if (info.state.type == horizon::kernel::MemoryType::Free) {
        info.ipc_ref_count = 0;
        info.device_ref_count = 0;
    } else {
        info.ipc_ref_count = 1;
        info.device_ref_count = 1;
    }

    return info;
}

vaddr_t IMmu::FindFreeMemory(range<vaddr_t> region, usize size) const {
    size = align(size, GUEST_PAGE_SIZE);
    auto crnt_region = range<vaddr_t>::FromSize(region.begin, size);
    while (region.Contains(crnt_region)) {
        const auto info = QueryMemory(crnt_region.begin);
        const auto mem_range = range<vaddr_t>(std::max(info.addr, region.begin),
                                              info.addr + info.size);
        if (info.state.type == horizon::kernel::MemoryType::Free &&
            mem_range.Contains(crnt_region))
            return mem_range.begin;

        crnt_region.Shift(mem_range.GetSize());
    }

    return 0x0;
}

} // namespace hydra::hw::tegra_x1::cpu
