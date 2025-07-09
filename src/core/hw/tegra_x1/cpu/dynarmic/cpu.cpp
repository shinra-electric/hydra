#include "core/hw/tegra_x1/cpu/dynarmic/cpu.hpp"

#include "core/hw/tegra_x1/cpu/dynarmic/memory.hpp"
#include "core/hw/tegra_x1/cpu/dynarmic/mmu.hpp"
#include "core/hw/tegra_x1/cpu/dynarmic/thread.hpp"

namespace hydra::hw::tegra_x1::cpu::dynarmic {

IMmu* Cpu::CreateMmu() { return new Mmu(); }

IThread* Cpu::CreateThread(IMmu* mmu, IMemory* tls_mem) {
    Thread* thread = new Thread(mmu, tls_mem);
    // TODO

    return thread;
}

IMemory* Cpu::AllocateMemory(usize size) {
    size = align(size, GUEST_PAGE_SIZE);
    auto memory = new Memory(size);

    return memory;
}

} // namespace hydra::hw::tegra_x1::cpu::dynarmic
