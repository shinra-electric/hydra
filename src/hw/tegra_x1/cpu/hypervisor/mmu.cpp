#include "hw/tegra_x1/cpu/hypervisor/mmu.hpp"

#include "hw/tegra_x1/cpu/hypervisor/const.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"

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
    HYP_ASSERT_SUCCESS(hv_vm_map(mem->GetPtrU8(), PAGE_TABLE_MEM_BASE,
                                 mem->GetSize(),
                                 PermisionToHV(mem->GetPermission())));
}

MMU::~MMU() {}

/*
void MMU::Reprotect(Memory* mem, uptr base) {
    HYP_ASSERT_SUCCESS(hv_vm_protect(base, mem->GetSize(),
                                     PermisionToHV(mem->GetPermission())));
}
*/

void MMU::MapImpl(uptr base, MemoryMapping mem) {
    HYP_ASSERT_SUCCESS(
        hv_vm_map(reinterpret_cast<void*>(mem.ptr), base, mem.size,
                  /*PermisionToHV(mem->GetPermission())*/ HV_MEMORY_READ |
                      HV_MEMORY_WRITE | HV_MEMORY_EXEC));

    // Page table
    // page_table.MapMemory(mem);
}

void MMU::UnmapImpl(uptr base, MemoryMapping mem) {
    HYP_ASSERT_SUCCESS(hv_vm_unmap(base, mem.size));

    // Page table
    // page_table.UnmapMemory(mem);
}

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
