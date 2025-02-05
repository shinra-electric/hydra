#include "hw/mmu/memory.hpp"

#define MEMORY_ALIGNMENT 0x4000

namespace Hydra::HW::MMU {

Memory::Memory(uptr base_, usize size_, Horizon::Permission permission_)
    : base{base_}, size{align(size_, (usize)MEMORY_ALIGNMENT)},
      permission{permission_} {
    posix_memalign((void**)(&ptr), MEMORY_ALIGNMENT, size);
    if (!ptr) {
        printf("Failed to allocate memory\n");
        return;
    }
}

Memory::~Memory() { free((void*)ptr); }

void Memory::Clear() { memset((void*)ptr, 0, size); }

uptr Memory::MapPtr(uptr p) { return base + (p - ptr); }

uptr Memory::UnmapPtr(uptr p) { return ptr + (p - base); }

} // namespace Hydra::HW::MMU
