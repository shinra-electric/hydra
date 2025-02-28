#include "hw/tegra_x1/cpu/hypervisor/page_table.hpp"

#define PTE_BLOCK (1ull << 0)
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

} // namespace

PageTable::PageTable() {
    // For now, only 1 level is used, creating a 1 to 1 mapping
    levels.resize(1);
    levels[0] = PageTableLevel(0, 30); // 1gb
    // levels[1] = PtLevel(1, 21, &levels[2]); // 2mb
    // levels[2] = PtLevel(2, 12);             // 4kb

    // Memory
    page_table_mem =
        new Memory(GetBlockCount() * sizeof(u64), Horizon::Permission::Read);
    page_table_mem->Clear();

    // Walk through the table
    u64* table = reinterpret_cast<u64*>(page_table_mem->GetPtr());
    const auto& level = levels[0];
    for (uptr addr = 0x0; addr < ADDRESS_SPACE_SIZE;
         addr += level.GetBlockSize()) {
        u64 value = addr | PTE_BLOCK | PTE_AF | PTE_INNER_SHEREABLE |
                    (u64)ApFlags::UserNoneKernelReadWriteExecute;

        table[GetPaOffset(level, addr)] = value;
    }
}

PageTable::~PageTable() { delete page_table_mem; }

void PageTable::MapMemory(Memory* mem) {
    // Access permission flags
    // ApFlags ap = mem->IsKernel() ? ApFlags::UserNoneKernelReadWriteExecute
    //                             : PermisionToAP(mem->GetPermission());

    // Walk through the table
    /*
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
    */
    LOG_WARNING(Hypervisor, "Not implemented");
}

void PageTable::UnmapMemory(Memory* mem) {
    LOG_WARNING(Hypervisor, "Not implemented");
}

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
