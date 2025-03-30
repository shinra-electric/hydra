#include "hw/tegra_x1/cpu/hypervisor/page_table.hpp"

#define PTE_TYPE_MASK 0x3ull
#define PTE_BLOCK(level) ((level == 2 ? 3ull : 1ull) << 0)
#define PTE_TABLE (3ull << 0)           // For level 0 and 1 descriptors
#define PTE_AF (1ull << 10)             // Access Flag
#define PTE_RW (1ull << 6)              // Read write
#define PTE_INNER_SHEREABLE (3ull << 8) // TODO: wht

/*
#define USER_RANGE_MEM_BASE 0x01000000
#define USER_RANGE_MEM_SIZE 0x1000000

#define KERNEL_RANGE_MEM_BASE 0x04000000
#define KERNEL_RANGE_MEM_SIZE 0x1000000
*/

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

namespace {

constexpr u64 ENTRY_ADDR_MASK = 0x0000fffffffff000; // TODO: correct?

// From Ryujinx
enum class ApFlags : u64 {
    ApShift = 6,
    PxnShift = 53,
    UxnShift = 54,

    UserExecuteKernelReadWriteExecute = (0UL << (int)ApShift),
    UserReadWriteExecuteKernelReadWrite = (1UL << (int)ApShift),
    UserExecuteKernelReadExecute = (2UL << (int)ApShift),
    UserReadExecuteKernelReadExecute = (3UL << (int)ApShift),

    UserExecuteKernelReadWrite = (1UL << (int)PxnShift) | (0UL << (int)ApShift),
    UserExecuteKernelRead = (1UL << (int)PxnShift) | (2UL << (int)ApShift),
    UserReadExecuteKernelRead = (1UL << (int)PxnShift) | (3UL << (int)ApShift),

    UserNoneKernelReadWriteExecute =
        (1UL << (int)UxnShift) | (0UL << (int)ApShift),
    UserReadWriteKernelReadWrite =
        (1UL << (int)UxnShift) | (1UL << (int)ApShift),
    UserNoneKernelReadExecute = (1UL << (int)UxnShift) | (2UL << (int)ApShift),
    UserReadKernelReadExecute = (1UL << (int)UxnShift) | (3UL << (int)ApShift),

    UserNoneKernelReadWrite =
        (1UL << (int)PxnShift) | (1UL << (int)UxnShift) | (0UL << (int)ApShift),
    UserNoneKernelRead =
        (1UL << (int)PxnShift) | (1UL << (int)UxnShift) | (2UL << (int)ApShift),
    UserReadKernelRead =
        (1UL << (int)PxnShift) | (1UL << (int)UxnShift) | (3UL << (int)ApShift),
};

/*
inline ApFlags PermisionToAP(Horizon::Permission permission) {
    return ApFlags::UserNoneKernelReadWriteExecute; // HACK: wtf why does this
                                                    // work

    if (any(permission & Horizon::Permission::Read)) {
        if (any(permission & Horizon::Permission::Write)) {
            if (any(permission & Horizon::Permission::Execute)) {
                return ApFlags::UserReadWriteExecuteKernelReadWrite;
            } else {
                return ApFlags::UserReadWriteKernelReadWrite;
            }
        } else {
            if (any(permission & Horizon::Permission::Execute)) {
                return ApFlags::UserReadExecuteKernelReadExecute;
            } else {
                return ApFlags::UserReadKernelReadExecute;
            }
        }
    } else {
        if (any(permission & Horizon::Permission::Write)) {
            if (any(permission & Horizon::Permission::Execute)) {
                return ApFlags::UserReadWriteExecuteKernelReadWrite;
            } else {
                return ApFlags::UserReadWriteKernelReadWrite;
            }
        } else {
            if (any(permission & Horizon::Permission::Execute)) {
                return ApFlags::UserExecuteKernelRead;
            } else {
                return ApFlags::UserNoneKernelReadWrite;
            }
        }
    }
}
*/

} // namespace

PageTableLevel::PageTableLevel(u32 level_, const Page page_,
                               const vaddr base_va_)
    : level{level_}, page{page_}, base_va{base_va_} {
    u64* table = reinterpret_cast<u64*>(page.ptr);
    for (u32 i = 0; i < ENTRY_COUNT; i++) {
        table[i] = 0x0; // 0x200000000 | PTE_BLOCK(level);
    }
}

PageTableLevel& PageTableLevel::GetNext(PageAllocator& allocator, u32 index) {
    ASSERT_DEBUG(level < 2, Hypervisor, "Level 2 is the last level");

    auto& next = next_levels[index];
    if (!next) {
        next = new PageTableLevel(level + 1, allocator.GetNextPage(),
                                  base_va + index * GetBlockSize());
        WriteEntry(index, next->page.pa | PTE_TABLE);
    }

    return *next;
}

PageTable::PageTable(paddr base_pa)
    : allocator(base_pa, 1024), top_level(0, allocator.GetNextPage(), 0x0) {}

PageTable::~PageTable() = default;

void PageTable::Map(vaddr va, paddr pa, usize size) {
    LOG_DEBUG(Hypervisor, "va: 0x{:08x}, pa: 0x{:08x}, size: 0x{:08x}", va, pa,
              size);

    ASSERT_ALIGNMENT(va, PAGE_SIZE, Hypervisor, "va");
    ASSERT_ALIGNMENT(pa, PAGE_SIZE, Hypervisor, "pa");
    ASSERT_ALIGNMENT(size, PAGE_SIZE, Hypervisor, "size");

    MapLevel(top_level, va, pa, size);
}

void PageTable::Unmap(vaddr va, usize size) {
    LOG_NOT_IMPLEMENTED(Hypervisor, "Memory unmapping");
}

// TODO: find out if there is a cheaper way
paddr PageTable::UnmapAddr(vaddr va) const {

    u32 index = top_level.VaToIndex(va);
    auto* level = &top_level;
    u64 entry = top_level.ReadEntry(index);
    while ((entry & PTE_TYPE_MASK) != PTE_BLOCK(level->GetLevel())) {
        if ((entry & PTE_TYPE_MASK) != PTE_TABLE)
            LOG_ERROR(Hypervisor, "Failed to unmap va 0x{:08x}", va);

        level = level->GetNextNoNew(index);
        index = level->VaToIndex(va);
        entry = level->ReadEntry(index);
    }

    return (entry & ENTRY_ADDR_MASK) + (va & (level->GetBlockSize() - 1));
}

void PageTable::MapLevel(PageTableLevel& level, vaddr va, paddr pa,
                         usize size) {
    vaddr end_va = va + size;
    do {
        MapLevelNext(
            level, va, pa,
            std::min(align(va + 1, level.GetBlockSize()) - va, end_va - va));

        vaddr old_va = va;
        va = align_down(va + level.GetBlockSize(), level.GetBlockSize());
        pa += va - old_va;
    } while (va < end_va);
}

void PageTable::MapLevelNext(PageTableLevel& level, vaddr va, paddr pa,
                             usize size) {
    // LOG_DEBUG(Hypervisor,
    //           "Level: {}, va: 0x{:08x}, pa: 0x{:08x}, size: 0x{:08x}",
    //           level.GetLevel(), va, pa, size);

    u32 index = level.VaToIndex(va);
    // TODO: uncomment
    if (/*size == level.GetBlockSize()*/ level.GetLevel() == 2) {
        // TODO: use proper permissions
        ApFlags ap;
        if (va >= 0x10000000 && va < 0x20000000)
            ap = ApFlags::UserReadWriteKernelReadWrite;
        else
            ap = ApFlags::UserNoneKernelReadWriteExecute;
        level.WriteEntry(index, pa | PTE_BLOCK(level.GetLevel()) | PTE_AF |
                                    PTE_INNER_SHEREABLE | (u64)ap);
    } else {
        MapLevel(level.GetNext(allocator, index), va, pa, size);
    }
}

/*
void PageTable::MapMemory(Memory* mem) {
    // Access permission flags
    // ApFlags ap = mem->IsKernel() ? ApFlags::UserNoneKernelReadWriteExecute
    //                             : PermisionToAP(mem->GetPermission());

    // Walk through the table
    u64* table = reinterpret_cast<u64*>(page_table_mem->GetPtr());
    for (const auto& level : levels) {
        auto next = level.GetNext();

        uptr start = mem->GetBase() & ~level.GetBlockMask(); // Round down
        uptr end = align(mem->GetBase() + mem->GetSize(),
                         level.GetBlockSize()); // Round up
        for (uptr addr = start; addr < end; addr += level.GetBlockSize()) {
            u64 value = 0;
            if (next) // Table
                value |= reinterpret_cast<u64>(
                             reinterpret_cast<u64*>(page_table_mem->GetBase()) +
                             GetPaOffset(*next, addr)) |
                         PTE_TABLE;
            else // Page
                value |=
                    addr | PTE_BLOCK | PTE_AF | PTE_INNER_SHEREABLE | (u64)ap;

            table[GetPaOffset(level, addr)] = value;
        }
    }
    LOG_WARNING(Hypervisor, "Not implemented");
}
*/

// void PageTable::UnmapMemory(Memory* mem) {
//     LOG_WARNING(Hypervisor, "Not implemented");
// }

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
