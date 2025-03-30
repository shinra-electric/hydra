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

const u32 exception_handler[] = {
    0xd41fffe2u, // hvc #0xFFFF
    // 0xd69f03e0u, // eret
    // 0xD2B00000, // mov x0, #0x80000000
    // 0xD61F0000, // br  x0
    // Shouldn't happen
    0xd4200000u, // brk #0
};

const u32 exception_trampoline[] = {
    0xd508831fu, // msr spsel, xzr

    // 0x910003e0,  // mov x0, sp
    // 0xd5384241,  // TODO
    // 0xd5384202,  // mrs x2, spsel
    // 0xD4200000u, // brk #0

    0xd69f03e0u, // eret
    // Shouldn't happen
    0xd4200000u, // brk #0
};

} // namespace

MMU::MMU() : user_page_table(0x100000000), kernel_page_table(0x120000000) {
    physical_memory_ptr = allocate_vm_memory(PHYSICAL_MEMORY_SIZE);
    HV_ASSERT_SUCCESS(hv_vm_map(
        reinterpret_cast<void*>(physical_memory_ptr), 0x0, PHYSICAL_MEMORY_SIZE,
        HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC));

    // Kernel memory
    uptr kernel_mem_ptr = physical_memory_ptr + physical_memory_cur;
    // TODO: map to kernel page table instead
    user_page_table.Map(KERNEL_REGION_BASE, physical_memory_cur,
                        KERNEL_MEM_SIZE);
    physical_memory_cur += KERNEL_MEM_SIZE;

    for (u64 offset = 0; offset < 0x780; offset += 0x80) {
        memcpy(reinterpret_cast<void*>(kernel_mem_ptr + offset),
               exception_handler, sizeof(exception_handler));
    }
    memcpy(
        reinterpret_cast<void*>(kernel_mem_ptr + EXCEPTION_TRAMPOLINE_OFFSET),
        exception_trampoline, sizeof(exception_trampoline));
}

MMU::~MMU() { free(reinterpret_cast<void*>(physical_memory_ptr)); }

uptr MMU::AllocateAndMap(vaddr va, usize size) {
    size = align(size, PAGE_SIZE);

    uptr ptr = physical_memory_ptr + physical_memory_cur;
    user_page_table.Map(va, physical_memory_cur, size);
    physical_memory_cur += size;

    return ptr;
}

void MMU::UnmapAndFree(vaddr va, usize size) {
    LOG_NOT_IMPLEMENTED(Hypervisor, "Memory unmapping");

    user_page_table.Unmap(va, size);
}

// TODO: just improve this...
void MMU::ResizeHeap(vaddr va, usize size) {
    paddr pa = user_page_table.UnmapAddr(va);
    user_page_table.Map(va, pa, size);
}

// HACK: this assumes that the whole src range is stored contiguously in
// physical memory
void MMU::Map(vaddr dst_va, vaddr src_va, usize size) {
    paddr pa = user_page_table.UnmapAddr(src_va);
    user_page_table.Map(dst_va, pa, size);
}

void MMU::Unmap(vaddr va, usize size) { user_page_table.Unmap(va, size); }

uptr MMU::UnmapAddr(vaddr va) const {
    return physical_memory_ptr + user_page_table.UnmapAddr(va);
}

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
