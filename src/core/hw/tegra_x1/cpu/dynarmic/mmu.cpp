#include "core/hw/tegra_x1/cpu/dynarmic/mmu.hpp"

#include "core/hw/tegra_x1/cpu/dynarmic/memory.hpp"

namespace hydra::hw::tegra_x1::cpu::dynarmic {

void Mmu::Map(vaddr_t dst_va, uptr ptr, usize size,
              const horizon::kernel::MemoryState state) {
    ASSERT_ALIGNMENT(size, GUEST_PAGE_SIZE, Dynarmic, "size");

    u64 va_page = dst_va / GUEST_PAGE_SIZE;
    u64 size_page = size / GUEST_PAGE_SIZE;
    u64 va_page_end = va_page + size_page;
    for (u64 page = va_page; page < va_page_end; ++page) {
        auto page_ptr = ptr + ((page - va_page) * GUEST_PAGE_SIZE);
        memory_ranges[page] = {page_ptr, state};
    }
}

void Mmu::Map(vaddr_t dst_va, vaddr_t src_va, usize size) {
    ASSERT_ALIGNMENT(size, GUEST_PAGE_SIZE, Dynarmic, "size");

    auto src_page = src_va / GUEST_PAGE_SIZE;
    auto dst_page = dst_va / GUEST_PAGE_SIZE;
    auto size_page = size / GUEST_PAGE_SIZE;
    for (u64 i = 0; i < size_page; i++) {
        memory_ranges[dst_page + i] = memory_ranges[src_page + i];
    }
}

void Mmu::Unmap(vaddr_t va, usize size) {
    ASSERT_ALIGNMENT(size, GUEST_PAGE_SIZE, Dynarmic, "size");

    auto va_page = va / GUEST_PAGE_SIZE;
    auto size_page = size / GUEST_PAGE_SIZE;
    auto va_page_end = va_page + size_page;
    for (u64 page = va_page; page < va_page_end; ++page) {
        memory_ranges.erase(page);
    }
}

void Mmu::ResizeHeap(IMemory* heap_mem, vaddr_t va, usize size) {
    auto mem_impl = static_cast<Memory*>(heap_mem);

    mem_impl->Resize(size);

    auto memory_ptr = mem_impl->GetPtr();

    u64 va_page = va / GUEST_PAGE_SIZE;
    u64 size_page = size / GUEST_PAGE_SIZE;
    u64 va_page_end = va_page + size_page;
    for (u64 page = va_page; page < va_page_end; ++page) {
        auto page_ptr = memory_ptr + ((page - va_page) * GUEST_PAGE_SIZE);
        memory_ranges[page] = {page_ptr, memory_ranges[va_page].state};
    }
}

uptr Mmu::UnmapAddr(vaddr_t va) const {
    auto page = va / GUEST_PAGE_SIZE;
    auto page_offset = va % GUEST_PAGE_SIZE;

    const auto it = memory_ranges.find(page);
    if (it == memory_ranges.end())
        return 0x0;

    return it->second.ptr + page_offset;
}

MemoryRegion Mmu::QueryRegion(vaddr_t va) const {
    const auto it = memory_ranges.find(va / GUEST_PAGE_SIZE);
    horizon::kernel::MemoryState state = {
        .type = horizon::kernel::MemoryType::Free};
    if (it != memory_ranges.end())
        state = it->second.state;

    return {
        .va = align_down(va, GUEST_PAGE_SIZE),
        .size = GUEST_PAGE_SIZE,
        .state = state,
    };
}

} // namespace hydra::hw::tegra_x1::cpu::dynarmic
