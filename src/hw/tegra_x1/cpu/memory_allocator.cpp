#include "hw/tegra_x1/cpu/memory_allocator.hpp"

#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::CPU {

MemoryAllocator::MemoryAllocator(MMUBase* mmu_) : mmu{mmu_} {
    constexpr usize MEMORY_SIZE = 4ull * 1024ull * 1024ull * 1024ull; // 4GB

    posix_memalign((void**)(&ptr), PAGE_SIZE, MEMORY_SIZE);
    if (!ptr) {
        LOG_ERROR(MMU, "Failed to allocate memory");
        return;
    }
    mmu->MapPhysicalMemory(ptr, 0x0, MEMORY_SIZE);

    // AllocatePages(4096);
    mmu->Allocate(*this);
}

MemoryAllocator::~MemoryAllocator() {
    free((void*)ptr);
    // TODO: free chunks
}

uptr MemoryAllocator::Allocate(usize size) {
    usize page_count = ceil_divide_u_pow2(size, PAGE_SHIFT);

    u64 page = page_counter;
    page_counter += page_count;

    AllocateImpl(page, page_count);

    return page * PAGE_SIZE;
}

void MemoryAllocator::AllocateExplicit(uptr pa, usize size) {
    u64 page = floor_divide_u_pow2(pa, PAGE_SHIFT);
    usize page_count = ceil_divide_u_pow2(size, PAGE_SHIFT);

    AllocateImpl(page, page_count);
}

void MemoryAllocator::AllocateImpl(u64 page, usize page_count) {
    /*
    u64 page_end = page + page_count;
    u64 allocated_pages = chunks.size() * PAGES_PER_CHUNK;
    if (page_end > allocated_pages) {
        AllocatePages(page_end - allocated_pages);
    }
    */
}

/*
MemoryChunk& MemoryAllocator::AllocateChunk() {
    u64 chunk_index = chunks.size();
    auto chunk = new MemoryChunk();
    chunks.push_back(chunk);

    uptr pa = chunk_index * PAGES_PER_CHUNK * PAGE_SIZE;
    mmu->MapPhysicalMemory(PaToPtr(pa), pa, PAGES_PER_CHUNK * PAGE_SIZE);

    return *chunk;
}

void MemoryAllocator::AllocatePages(usize count) {
    count = align(count, PAGES_PER_CHUNK);
    usize chunk_count = count / PAGES_PER_CHUNK;
    for (usize i = 0; i < chunk_count; i++) {
        AllocateChunk();
    }
}
*/

} // namespace Hydra::HW::TegraX1::CPU
