#include "hw/tegra_x1/mmu/memory.hpp"

#define MEMORY_ALIGNMENT 0x4000

namespace Hydra::HW::TegraX1::MMU {

Memory::Memory(uptr base_, usize size_, Horizon::Permission permission_)
    : base{base_}, size{align(size_, (usize)MEMORY_ALIGNMENT)},
      permission{permission_} {
    Allocate();
}

Memory::~Memory() { free((void*)ptr); }

void Memory::Resize(usize size_) {
    uptr old_ptr = ptr;
    usize old_size = size;

    size = align(size_, (usize)MEMORY_ALIGNMENT);
    Allocate();
    // TODO: is this necessary?
    memcpy((void*)ptr, (void*)old_ptr, std::min(size, old_size));

    // Cleanup
    free((void*)old_ptr);
}

void Memory::Clear() { memset((void*)ptr, 0, size); }

uptr Memory::MapPtr(uptr p) { return base + (p - ptr); }

uptr Memory::UnmapAddr(uptr p) { return ptr + (p - base); }

bool Memory::AddrIsInRange(uptr p) { return p >= base && p < base + size; }

void Memory::Allocate() {
    posix_memalign((void**)(&ptr), MEMORY_ALIGNMENT, size);
    if (!ptr) {
        LOG_ERROR(MMU, "Failed to allocate memory");
        return;
    }
}

} // namespace Hydra::HW::TegraX1::MMU
