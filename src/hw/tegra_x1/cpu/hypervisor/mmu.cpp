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

} // namespace

MMU::MMU() {
    auto mem = page_table.GetMemory();
    HYP_ASSERT_SUCCESS(hv_vm_map(mem->GetPtrU8(), mem->GetBase(),
                                 mem->GetSize(),
                                 PermisionToHV(mem->GetPermission())));
}

MMU::~MMU() {}

void MMU::ReprotectMemory(Memory* mem) {
    HYP_ASSERT_SUCCESS(hv_vm_protect(mem->GetBase(), mem->GetSize(),
                                     PermisionToHV(mem->GetPermission())));
}

void MMU::MapMemoryImpl(Memory* mem) {
    HYP_ASSERT_SUCCESS(hv_vm_map(mem->GetPtrU8(), mem->GetBase(),
                                 mem->GetSize(),
                                 PermisionToHV(mem->GetPermission())));

    // Page table
    page_table.MapMemory(mem);
}

void MMU::UnmapMemoryImpl(Memory* mem) {
    HYP_ASSERT_SUCCESS(hv_vm_unmap(mem->GetBase(), mem->GetSize()));

    // Page table
    page_table.UnmapMemory(mem);
}

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
