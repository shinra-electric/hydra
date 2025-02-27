#pragma once

#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

constexpr usize ADDRESS_SPACE_SIZE = (1ul << 39);

struct PtLevel {
    PtLevel() = default;
    PtLevel(u32 index_, u32 block_shift_, PtLevel* next_ = nullptr)
        : index{index_}, block_shift{block_shift_}, next{next_} {}

    uptr GetPaToOffset(uptr pa) const { return pa >> block_shift; }

    usize GetBlockSize() const { return 1 << block_shift; }

    usize GetBlockCount() const { return ADDRESS_SPACE_SIZE / GetBlockSize(); }

    // Getters
    u32 GetIndex() const { return index; }

    PtLevel* GetNext() const { return next; }

  private:
    u32 index;
    u32 block_shift;
    PtLevel* next;
};

class PtManager {
  public:
    PtManager() {
        levels.resize(3);
        levels[0] = PtLevel(0, 30, &levels[1]); // 1gb
        levels[1] = PtLevel(1, 21, &levels[2]); // 2mb
        levels[2] = PtLevel(2, 12);             // 4kb
    }

    usize GetBlockCount() const {
        usize count = 0;
        for (const auto& level : levels) {
            count += level.GetBlockCount();
        }

        return count;
    }

    u64 GetPtLevelOffset(u32 index) const {
        u64 offset = 0;
        for (u32 i = 0; i < index; i++) {
            offset += levels[i].GetBlockCount();
        }

        return offset;
    }

    u64 GetPtLevelOffset(const PtLevel& level) const {
        return GetPtLevelOffset(level.GetIndex());
    }

    u64 GetPaOffset(const PtLevel& level, uptr pa) const {
        return GetPtLevelOffset(level.GetIndex()) + level.GetPaToOffset(pa);
    }

    // Getters
    const std::vector<PtLevel>& GetPtLevels() const { return levels; }

  private:
    std::vector<PtLevel> levels;
};

class MMU : public MMUBase {
  public:
    MMU();
    ~MMU();

    void ReprotectMemory(Memory* mem) override;

    // Getters
    Memory* GetPtMemory() const { return pt_mem; }
    // Memory* GetKernelRangeMemory() const { return kernel_range.mem; }

  protected:
    void MapMemoryImpl(Memory* mem) override;
    void UnmapMemoryImpl(Memory* mem) override;

  private:
    // Page table
    PtManager pt_manager;

    // Memory
    Memory* pt_mem;
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
