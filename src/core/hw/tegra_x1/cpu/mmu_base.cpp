#include "core/hw/tegra_x1/cpu/mmu_base.hpp"

namespace hydra::hw::tegra_x1::cpu {

SINGLETON_DEFINE_GET_INSTANCE(MMUBase, MMU)

MMUBase::MMUBase() { SINGLETON_SET_INSTANCE(MMU, MMU); }

MMUBase::~MMUBase() { SINGLETON_UNSET_INSTANCE(); }

horizon::kernel::MemoryInfo MMUBase::QueryMemory(vaddr_t va) const {
    horizon::kernel::MemoryInfo info;
    info.size = 0x0;

    auto region = QueryRegion(va);

    // Resize to the left
    do {
        info.addr = region.va;
        info.size = region.size;
        info.state = region.state;
        if (info.addr == 0x0)
            break;

        region = QueryRegion(info.addr - 1);
    } while (region.state == info.state);

    // Resize to the right
    do {
        vaddr_t addr = info.addr + info.size;
        if (addr >= horizon::kernel::ADDRESS_SPACE_END)
            break;

        region = QueryRegion(addr);
        info.size += region.size;
    } while (region.state == info.state);

    return info;
}

} // namespace hydra::hw::tegra_x1::cpu
