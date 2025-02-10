#include "hw/tegra_x1/mmu/mmu.hpp"

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

Memory* MMUBase::UnmapPtrToMemory(uptr ptr) {
    for (Memory* mem : memories) {
        if (ptr >= mem->GetBase() && ptr < mem->GetBase() + mem->GetSize()) {
            return mem;
        }
    }

    LOG_ERROR(MMU, "Failed to unmap ptr 0x{:08x}", ptr);

    return nullptr;
}

uptr MMUBase::UnmapPtr(uptr ptr) {
    return UnmapPtrToMemory(ptr)->UnmapPtr(ptr);
}

} // namespace Hydra::HW::MMU
