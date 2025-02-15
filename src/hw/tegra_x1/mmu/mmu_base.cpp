#include "hw/tegra_x1/mmu/mmu_base.hpp"

#include "hw/tegra_x1/mmu/memory.hpp"

namespace Hydra::HW::MMU {

void MMUBase::MapMemory(Memory* mem) {
    memories.push_back(mem);
    MapMemoryImpl(mem);
}

void MMUBase::UnmapMemory(Memory* mem) {
    std::vector<Memory*>::iterator it = memories.begin();
    while (it != memories.end()) {
        if (*it == mem) {
            memories.erase(it);
            UnmapMemoryImpl(mem);
            return;
        }
        it++;
    }

    throw std::runtime_error("Memory not found");
}

void MMUBase::RemapMemory(Memory* mem) {
    // We can just call the subclass implementations, since we don't want to
    // remove the memory from the list
    UnmapMemoryImpl(mem);
    MapMemoryImpl(mem);
}

Memory* MMUBase::UnmapAddrToMemory(uptr addr) const {
    for (Memory* mem : memories) {
        if (addr >= mem->GetBase() && addr < mem->GetBase() + mem->GetSize()) {
            return mem;
        }
    }

    LOG_ERROR(MMU, "Failed to unmap addr 0x{:08x}", addr);

    return nullptr;
}

uptr MMUBase::UnmapAddr(uptr addr) const {
    Memory* mem = UnmapAddrToMemory(addr);
    if (!mem)
        return 0;

    return mem->UnmapAddr(addr);
}

} // namespace Hydra::HW::MMU
