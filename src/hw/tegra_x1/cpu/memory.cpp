#include "hw/tegra_x1/cpu/memory.hpp"

#include "hw/tegra_x1/cpu/memory_allocator.hpp"

namespace Hydra::HW::TegraX1::CPU {

Memory::Memory(MemoryAllocator& allocator_, uptr base_, usize size_,
               Horizon::Permission permission_, bool is_kernel_)
    : allocator{allocator_}, base{base_}, size{size_}, permission{permission_},
      is_kernel{is_kernel_} {
    pa = allocator.Allocate(size);
}

Memory::~Memory() {}

// Should only be called on heap memory
void Memory::Resize(usize size_) { allocator.AllocateExplicit(pa, size); }

// uptr Memory::MapPtr(uptr p) { return base + (p - ptr); }

uptr Memory::UnmapAddr(uptr addr) {
    return allocator.PaToPtr(pa + (addr - base));
}

bool Memory::AddrIsInRange(uptr addr) {
    return addr >= base && addr < base + size;
}

uptr Memory::GetPtr() const { return allocator.PaToPtr(pa); }

u8* Memory::GetPtrU8() const { return reinterpret_cast<u8*>(GetPtr()); }

} // namespace Hydra::HW::TegraX1::CPU
