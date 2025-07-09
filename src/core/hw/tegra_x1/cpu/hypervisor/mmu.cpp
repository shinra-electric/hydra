#include "core/hw/tegra_x1/cpu/hypervisor/mmu.hpp"

#include "core/debugger/debugger.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/const.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/cpu.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/memory.hpp"

/*
#define USER_RANGE_MEM_BASE 0x01000000
#define USER_RANGE_MEM_SIZE 0x1000000

#define KERNEL_RANGE_MEM_BASE 0x04000000
#define KERNEL_RANGE_MEM_SIZE 0x1000000
*/

#define CPU (*static_cast<Cpu*>(&CPU_INSTANCE))

namespace hydra::hw::tegra_x1::cpu::hypervisor {

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

inline ApFlags to_ap_flags(horizon::kernel::MemoryPermission perm) {
    // HACK
    return ApFlags::UserReadWriteExecuteKernelReadWrite;

    if (any(perm & horizon::kernel::MemoryPermission::Read)) {
        if (any(perm & horizon::kernel::MemoryPermission::Write)) {
            if (any(perm & horizon::kernel::MemoryPermission::Execute)) {
                return ApFlags::UserReadWriteExecuteKernelReadWrite;
            } else {
                return ApFlags::UserReadWriteKernelReadWrite;
            }
        } else {
            if (any(perm & horizon::kernel::MemoryPermission::Execute)) {
                return ApFlags::UserReadExecuteKernelRead;
            } else {
                return ApFlags::UserReadKernelRead;
            }
        }
    } else {
        if (any(perm & horizon::kernel::MemoryPermission::Write)) {
            if (any(perm & horizon::kernel::MemoryPermission::Execute)) {
                return ApFlags::UserReadWriteExecuteKernelReadWrite; // TODO
            } else {
                return ApFlags::UserReadWriteKernelReadWrite; // TODO
            }
        } else {
            if (any(perm & horizon::kernel::MemoryPermission::Execute)) {
                return ApFlags::UserExecuteKernelRead; // TODO
            } else {
                return ApFlags::UserNoneKernelRead; // TODO
            }
        }
    }
}

} // namespace

Mmu::Mmu()
    : user_page_table(PHYSICAL_MEMORY_SIZE),
      kernel_page_table(PHYSICAL_MEMORY_SIZE + 0x20000000),
      kernel_mem(CPU.GetPAMapper(), align(KERNEL_MEM_SIZE, APPLE_PAGE_SIZE)) {
    // Kernel memory
    kernel_page_table.Map(0x0, CPU.GetPAMapper().GetPA(kernel_mem.GetPtr()),
                          KERNEL_MEM_SIZE,
                          {horizon::kernel::MemoryType::Kernel,
                           horizon::kernel::MemoryAttribute::None,
                           horizon::kernel::MemoryPermission::Execute},
                          ApFlags::UserNoneKernelReadExecute);

    for (u64 offset = 0; offset < 0x780; offset += 0x80) {
        memcpy(reinterpret_cast<void*>(kernel_mem.GetPtr() + offset),
               exception_handler, sizeof(exception_handler));
    }
    memcpy(reinterpret_cast<void*>(kernel_mem.GetPtr() +
                                   EXCEPTION_TRAMPOLINE_OFFSET),
           exception_trampoline, sizeof(exception_trampoline));

    // Loader return address
    // TODO: this should be done in a backend agnostic way (perhaps in the
    // kernel?)
    /*
    uptr ret_mem_ptr = physical_memory_ptr + physical_memory_cur;
    user_page_table.Map(0xffff0000, physical_memory_cur, 0x1000,
                        {horizon::kernel::MemoryType::Code,
                         horizon::kernel::MemoryAttribute::None,
                         horizon::kernel::MemoryPermission::Execute},
                        ApFlags::UserExecuteKernelRead);
    physical_memory_cur += 0x1000;

    *reinterpret_cast<u32*>(ret_mem_ptr) = 0xd40000e1; // svcExitProcess
    */

    // Symbols
    DEBUGGER_INSTANCE.GetModuleTable().RegisterSymbol(
        {"Hypervisor::handler",
         range<vaddr_t>(KERNEL_REGION_BASE,
                        KERNEL_REGION_BASE + EXCEPTION_TRAMPOLINE_OFFSET)});
    DEBUGGER_INSTANCE.GetModuleTable().RegisterSymbol(
        {"Hypervisor::trampoline",
         range<vaddr_t>(KERNEL_REGION_BASE + EXCEPTION_TRAMPOLINE_OFFSET,
                        KERNEL_REGION_BASE + EXCEPTION_TRAMPOLINE_OFFSET +
                            sizeof(exception_trampoline))});
}

Mmu::~Mmu() {}

void Mmu::Map(vaddr_t va, usize size, IMemory* memory,
              const horizon::kernel::MemoryState state) {
    ASSERT_ALIGNMENT(size, GUEST_PAGE_SIZE, Hypervisor, "size");
    user_page_table.Map(
        va, CPU.GetPAMapper().GetPA(static_cast<Memory*>(memory)->GetPtr()),
        size, state, to_ap_flags(state.perm));
}

// HACK: this assumes that the whole src range is stored contiguously in
// physical memory
void Mmu::Map(vaddr_t dst_va, vaddr_t src_va, usize size) {
    const auto region = user_page_table.QueryRegion(src_va);
    paddr_t pa = region.UnmapAddr(src_va);
    user_page_table.Map(dst_va, pa, size, region.state,
                        to_ap_flags(region.state.perm));
}

void Mmu::Unmap(vaddr_t va, usize size) { user_page_table.Unmap(va, size); }

// TODO: just improve this...
void Mmu::ResizeHeap(IMemory* heap_mem, vaddr_t va, usize size) {
    const auto region = user_page_table.QueryRegion(va);
    paddr_t pa = region.UnmapAddr(va);
    user_page_table.Map(va, pa, size, region.state,
                        to_ap_flags(region.state.perm));
}

uptr Mmu::UnmapAddr(vaddr_t va) const {
    return CPU.GetPAMapper().GetPtr(user_page_table.UnmapAddr(va));
}

MemoryRegion Mmu::QueryRegion(vaddr_t va) const {
    auto region = user_page_table.QueryRegion(va);

    return {
        .va = region.va,
        .size = region.size,
        .state = region.state,
    };
}

} // namespace hydra::hw::tegra_x1::cpu::hypervisor
