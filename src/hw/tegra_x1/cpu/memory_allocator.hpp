#pragma once

#include "common/common.hpp"

namespace Hydra::HW::TegraX1::CPU {

class MMUBase;

constexpr u32 PAGE_SHIFT = 14;
constexpr usize PAGE_SIZE = 1ul << PAGE_SHIFT; // 16kb
// constexpr usize PAGES_PER_CHUNK = 1024;

/*
struct MemoryChunk {
    uptr ptr;

    MemoryChunk() {
        posix_memalign((void**)(&ptr), PAGE_SIZE, PAGE_SIZE * PAGES_PER_CHUNK);
        if (!ptr) {
            LOG_ERROR(MMU, "Failed to allocate memory chunk");
            return;
        }

        memset((void*)ptr, 0, PAGE_SIZE * PAGES_PER_CHUNK);
    }

    ~MemoryChunk() { free((void*)ptr); }
};
*/

class MemoryAllocator {
  public:
    MemoryAllocator(MMUBase* mmu_);
    ~MemoryAllocator();

    uptr Allocate(usize size); // Return physical address
    void AllocateExplicit(uptr pa, usize size);

    // Lookup
    uptr PaToPtr(uptr pa) const {
        /*
        auto chunk = chunks[pa / (PAGE_SIZE * PAGES_PER_CHUNK)];
        uptr offset = pa % (PAGE_SIZE * PAGES_PER_CHUNK);

        return chunk->ptr + offset;
        */
        return ptr + pa;
    }

  private:
    MMUBase* mmu;

    uptr ptr;
    // std::vector<MemoryChunk*> chunks;
    usize page_counter = 0;

    void AllocateImpl(u64 page, usize page_count);

    // MemoryChunk& AllocateChunk();

    // void AllocatePages(usize count);

    // Lookup
    uptr PageToPa(u64 page) const { return page * PAGE_SIZE; }
};

} // namespace Hydra::HW::TegraX1::CPU
