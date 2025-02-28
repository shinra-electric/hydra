#include "hw/tegra_x1/cpu/hypervisor/mmu.hpp"

#include "hw/tegra_x1/cpu/hypervisor/const.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"

#define PTE_BLOCK (1ull << 0)
#define PTE_TABLE (3ull << 0)           // For level 0 and 1 descriptors
#define PTE_AF (1ull << 10)             // Access Flag
#define PTE_RW (1ull << 6)              // Read write
#define PTE_INNER_SHEREABLE (3ull << 8) // TODO: wht

#define PT_MEM_BASE 0x00000000a0000000

/*
#define USER_RANGE_MEM_BASE 0x01000000
#define USER_RANGE_MEM_SIZE 0x1000000

#define KERNEL_RANGE_MEM_BASE 0x04000000
#define KERNEL_RANGE_MEM_SIZE 0x1000000
*/

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

namespace {

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

inline hv_memory_flags_t PermisionToHV(Horizon::Permission permission) {
    hv_memory_flags_t flags = 0;
    if (any(permission & Horizon::Permission::Read))
        flags |= HV_MEMORY_READ;
    if (any(permission & Horizon::Permission::Write))
        flags |= HV_MEMORY_WRITE;
    if (any(permission & Horizon::Permission::Execute))
        flags |= HV_MEMORY_EXEC;

    return flags;
}

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

MMU::MMU() {
    // Memory
    pt_mem = new Memory(PT_MEM_BASE, pt_manager.GetBlockCount() * sizeof(u64),
                        Horizon::Permission::Read);
    pt_mem->Clear();
    HYP_ASSERT_SUCCESS(hv_vm_map(pt_mem->GetPtrU8(), pt_mem->GetBase(),
                                 pt_mem->GetSize(),
                                 PermisionToHV(pt_mem->GetPermission())));
}

MMU::~MMU() { delete pt_mem; }

void MMU::ReprotectMemory(Memory* mem) {
    HYP_ASSERT_SUCCESS(hv_vm_protect(mem->GetBase(), mem->GetSize(),
                                     PermisionToHV(mem->GetPermission())));
}

void MMU::MapMemoryImpl(Memory* mem) {
    HYP_ASSERT_SUCCESS(hv_vm_map(mem->GetPtrU8(), mem->GetBase(),
                                 mem->GetSize(),
                                 PermisionToHV(mem->GetPermission())));

    // Page table

    // Access permission flags
    ApFlags ap = mem->IsKernel() ? ApFlags::UserNoneKernelReadWriteExecute
                                 : PermisionToAP(mem->GetPermission());

    // Walk through the table
    u64* table = reinterpret_cast<u64*>(pt_mem->GetPtr());
    for (const auto& pt_level : pt_manager.GetPtLevels()) {
        auto next = pt_level.GetNext();

        uptr start = mem->GetBase() & ~pt_level.GetBlockMask(); // Round down
        uptr end = align(mem->GetBase() + mem->GetSize(),
                         pt_level.GetBlockSize()); // Round up
        for (uptr addr = start; addr < end; addr += pt_level.GetBlockSize()) {
            u64 value = 0;
            if (next) // Table
                value |= reinterpret_cast<u64>(
                             reinterpret_cast<u64*>(pt_mem->GetBase()) +
                             pt_manager.GetPaOffset(*next, addr)) |
                         PTE_TABLE;
            else // Page
                value |=
                    addr | PTE_BLOCK | PTE_AF | PTE_INNER_SHEREABLE | (u64)ap;

            table[pt_manager.GetPaOffset(pt_level, addr)] = value;
        }
    }
}

void MMU::UnmapMemoryImpl(Memory* mem) {
    HYP_ASSERT_SUCCESS(hv_vm_unmap(mem->GetBase(), mem->GetSize()));

    // TODO
}

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
