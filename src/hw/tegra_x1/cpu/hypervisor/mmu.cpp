#include "hw/tegra_x1/cpu/hypervisor/mmu.hpp"

#include "hw/tegra_x1/cpu/hypervisor/const.hpp"

/*
#define USER_RANGE_MEM_BASE 0x01000000
#define USER_RANGE_MEM_SIZE 0x1000000

#define KERNEL_RANGE_MEM_BASE 0x04000000
#define KERNEL_RANGE_MEM_SIZE 0x1000000
*/

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

namespace {

constexpr usize PHYSICAL_MEMORY_SIZE = 0x100000000; // 4GB

} // namespace

MMU::MMU() { physical_memory_ptr = allocate_vm_memory(PHYSICAL_MEMORY_SIZE); }

MMU::~MMU() { free(reinterpret_cast<void*>(physical_memory_ptr)); }

uptr MMU::AllocateAndMap(vaddr va, usize size) {
    size = align(size, PAGE_SIZE);

    uptr ptr = physical_memory_ptr + physical_memory_cur;
    page_table.Map(va, physical_memory_cur, size);
    physical_memory_cur += size;

    return ptr;
}

void MMU::UnmapAndFree(vaddr va, usize size) {
    LOG_NOT_IMPLEMENTED(Hypervisor, "Memory unmapping");

    page_table.Unmap(va, size);
}

uptr MMU::UnmapAddr(vaddr va) const {
    return physical_memory_ptr + page_table.UnmapAddr(va);
}

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
