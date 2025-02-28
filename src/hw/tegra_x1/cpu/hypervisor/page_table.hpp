#pragma once

#include "hw/tegra_x1/cpu/memory.hpp"

// TODO: needs rework

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

constexpr usize ADDRESS_SPACE_SIZE = (1ul << 39);
constexpr uptr PAGE_TABLE_MEM_BASE = 0x00000000a0000000;

struct PageTableLevel {
    PageTableLevel() = default;
    PageTableLevel(u32 index_, u32 block_shift_,
                   PageTableLevel* next_ = nullptr)
        : index{index_}, block_shift{block_shift_}, next{next_} {}

    uptr GetPaOffset(uptr pa) const { return pa >> block_shift; }

    usize GetBlockSize() const { return 1ul << block_shift; }

    u64 GetBlockMask() const { return GetBlockSize() - 1; }

    usize GetBlockCount() const { return ADDRESS_SPACE_SIZE >> block_shift; }

    // Getters
    u32 GetIndex() const { return index; }

    u32 GetBlockShift() const { return block_shift; }

    PageTableLevel* GetNext() const { return next; }

  private:
    u32 index;
    u32 block_shift;
    PageTableLevel* next;
};

class PageTable {
  public:
    PageTable();
    ~PageTable();

    void MapMemory(Memory* mem);
    void UnmapMemory(Memory* mem);

    // Getters
    Memory* GetMemory() const { return page_table_mem; }

  private:
    std::vector<PageTableLevel> levels;

    Memory* page_table_mem;

    usize GetBlockCount() const {
        usize count = 0;
        for (const auto& level : levels) {
            count += level.GetBlockCount();
        }

        return count;
    }

    u64 GetLevelOffset(u32 index) const {
        u64 offset = 0;
        for (u32 i = 0; i < index; i++) {
            offset += levels[i].GetBlockCount();
        }

        return offset;
    }

    u64 GetLevelOffset(const PageTableLevel& level) const {
        return GetLevelOffset(level.GetIndex());
    }

    u64 GetPaOffset(const PageTableLevel& level, uptr pa) const {
        return GetLevelOffset(level) + level.GetPaOffset(pa);
    }
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
