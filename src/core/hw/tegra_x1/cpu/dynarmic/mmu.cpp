#include "core/hw/tegra_x1/cpu/dynarmic/mmu.hpp"

#include "core/hw/tegra_x1/cpu/dynarmic/memory.hpp"

namespace Hydra::HW::TegraX1::CPU::Dynarmic {

MMU::MMU() {}

MMU::~MMU() {}

MemoryBase* MMU::AllocateMemory(usize size) {
    size = align(size, PAGE_SIZE);
    auto memory = new Memory(size);

    return memory;
}

void MMU::FreeMemory(MemoryBase* memory) { delete memory; }

uptr MMU::GetMemoryPtr(MemoryBase* memory) const {
    return static_cast<Memory*>(memory)->GetPtr();
}

void MMU::Map(vaddr va, usize size, MemoryBase* memory,
              const Horizon::MemoryState state) {
    ASSERT_ALIGNMENT(size, PAGE_SIZE, Dynarmic, "size");

    auto memory_ptr = static_cast<Memory*>(memory)->GetPtr();

    u64 va_page = va / PAGE_SIZE;
    u64 size_page = size / PAGE_SIZE;
    u64 va_page_end = va_page + size_page;
    for (u64 page = va_page; page < va_page_end; ++page) {
        auto page_ptr = memory_ptr + ((page - va_page) * PAGE_SIZE);
        pages[page] = page_ptr;
    }
}

void MMU::Map(vaddr dst_va, vaddr src_va, usize size) {
    ASSERT_ALIGNMENT(size, PAGE_SIZE, Dynarmic, "size");

    auto src_page = src_va / PAGE_SIZE;
    auto dst_page = dst_va / PAGE_SIZE;
    auto size_page = size / PAGE_SIZE;
    auto src_page_end = src_page + size_page;
    for (u64 page = 0; page < size_page; ++page) {
        pages[dst_page + page] = pages[src_page + page];
    }
}

void MMU::Unmap(vaddr va, usize size) {
    ASSERT_ALIGNMENT(size, PAGE_SIZE, Dynarmic, "size");

    auto va_page = va / PAGE_SIZE;
    auto size_page = size / PAGE_SIZE;
    auto va_page_end = va_page + size_page;
    for (u64 page = va_page; page < va_page_end; ++page) {
        pages[page] = 0x0;
    }
}

void MMU::ResizeHeap(MemoryBase* heap_mem, vaddr va, usize size) {
    auto mem_impl = static_cast<Memory*>(heap_mem);

    mem_impl->Resize(size);

    auto memory_ptr = mem_impl->GetPtr();

    u64 va_page = va / PAGE_SIZE;
    u64 size_page = size / PAGE_SIZE;
    u64 va_page_end = va_page + size_page;
    for (u64 page = va_page; page < va_page_end; ++page) {
        auto page_ptr = memory_ptr + ((page - va_page) * PAGE_SIZE);
        pages[page] = page_ptr;
    }
}

uptr MMU::UnmapAddr(vaddr va) const {
    auto page = va / PAGE_SIZE;
    auto page_offset = va % PAGE_SIZE;

    if (pages[page] == 0x0) {
        LOG_ERROR(Dynarmic, "Failed to unmap va 0x{:08x}", va);
        return 0x0;
    }

    return pages[page] + page_offset;
}

Horizon::MemoryInfo MMU::QueryMemory(vaddr va) const {
    LOG_NOT_IMPLEMENTED(Dynarmic, "Memory querying");

    // HACK
    return Horizon::MemoryInfo{
        .addr = va,
        .size = PAGE_SIZE,
    };
}

} // namespace Hydra::HW::TegraX1::CPU::Dynarmic
