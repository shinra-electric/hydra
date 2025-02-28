#include "hw/tegra_x1/cpu/mmu_base.hpp"

#include "hw/tegra_x1/cpu/memory.hpp"

namespace Hydra::HW::TegraX1::CPU {

void MMUBase::Map(Memory* mem, uptr base) {
    mapped_ranges[base] = mem;
    MapImpl(mem, base);
}

void MMUBase::Unmap(uptr base) {
    auto mem = mapped_ranges.at(base);
    UnmapImpl(mem, base);
}

void MMUBase::Remap(uptr base) {
    auto mem = mapped_ranges.at(base);

    // We can just call the subclass implementations, since we don't want to
    // remove the memory from the list
    UnmapImpl(mem, base);
    MapImpl(mem, base);
}

Memory* MMUBase::FindMemoryForAddr(uptr addr, uptr& out_base) const {
    for (auto [base, mem] : mapped_ranges) {
        if (addr >= base && addr < base + mem->GetSize()) {
            out_base = base;
            return mem;
        }
    }

    LOG_ERROR(MMU, "Failed to find memory for addr 0x{:08x}", addr);

    return nullptr;
}

uptr MMUBase::UnmapAddr(uptr addr) const {
    uptr base;
    Memory* mem = FindMemoryForAddr(addr, base);
    if (!mem)
        return 0x0;

    return reinterpret_cast<uptr>(mem->GetPtrU8() + (addr - base));
}

} // namespace Hydra::HW::TegraX1::CPU
