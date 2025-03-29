#pragma once

#include "hw/tegra_x1/cpu/hypervisor/page_allocator.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

constexpr usize BLOCK_SHIFT_DIFF = 9;
constexpr usize ENTRY_COUNT = 1u << BLOCK_SHIFT_DIFF;

#define GET_BLOCK_SHIFT(level) (3 + (BLOCK_SHIFT_DIFF * (3 - (level))))

constexpr usize ADDRESS_SPACE_SIZE = (1ul << GET_BLOCK_SHIFT(0));
constexpr uptr PAGE_TABLE_MEM_BASE_PA = 0x00000000a0000000;

struct PageTableLevel {
    PageTableLevel(u32 level_, const Page page_, const vaddr base_va_)
        : level{level_}, page{page_}, base_va{base_va_} {
        u64* table = reinterpret_cast<u64*>(page.ptr);
        for (u32 i = 0; i < ENTRY_COUNT; i++) {
            table[i] = 0x0;
        }
    }

    usize GetBlockSize() const { return 1ul << GET_BLOCK_SHIFT(level); }

    uptr PaToIndex(uptr pa) const {
        return (pa - page.pa) >> GET_BLOCK_SHIFT(level);
    }

    u32 VaToIndex(vaddr va) const {
        return (va - base_va) >> GET_BLOCK_SHIFT(level);
    }

    u64 ReadEntry(u32 index) const {
        const u64* table = reinterpret_cast<const u64*>(page.ptr);
        return table[index];
    }

    void WriteEntry(u32 index, u64 entry) {
        u64* table = reinterpret_cast<u64*>(page.ptr);
        table[index] = entry;
    }

    const PageTableLevel* GetNextNoNew(u32 index) const {
        return next_levels[index];
    }

    PageTableLevel& GetNext(PageAllocator& allocator, u32 index);

    // Getters
    u32 GetLevel() const { return level; }

    u32 GetBlockShift() const { return GET_BLOCK_SHIFT(level); }

  private:
    u32 level;
    const Page page;
    const vaddr base_va;
    PageTableLevel* next_levels[ENTRY_COUNT] = {nullptr};
};

class PageTable {
  public:
    PageTable();
    ~PageTable();

    void Map(vaddr va, paddr pa, usize size);
    void Unmap(vaddr va, usize size);

    paddr UnmapAddr(vaddr va) const;

  private:
    PageAllocator allocator;
    PageTableLevel top_level;

    void MapLevel(PageTableLevel& level, vaddr va, paddr pa, usize size);
    void MapLevelNext(PageTableLevel& level, vaddr va, paddr pa, usize size);
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
