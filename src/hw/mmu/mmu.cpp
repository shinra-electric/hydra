#include "hw/mmu/mmu.hpp"

#include "hw/mmu/memory.hpp"

namespace Hydra::HW::MMU {

void MMUBase::MapMemory(Memory* mem) { memories.push_back(mem); }

void MMUBase::UnmapMemory(Memory* mem) {
    std::vector<Memory*>::iterator it = memories.begin();
    while (it != memories.end()) {
        if (*it == mem) {
            memories.erase(it);
            return;
        }
        it++;
    }

    throw std::runtime_error("Memory not found");
}

Memory* MMUBase::UnmapPtrToMemory(uptr ptr) {
    for (Memory* mem : memories) {
        if (ptr >= mem->GetBase() && ptr < mem->GetBase() + mem->GetSize()) {
            return mem;
        }
    }

    printf("Failed to unmap ptr: 0x%lx\n", ptr);

    return nullptr;
}

uptr MMUBase::UnmapPtr(uptr ptr) {
    return UnmapPtrToMemory(ptr)->UnmapPtr(ptr);
}

} // namespace Hydra::HW::MMU
