#include "hw/tegra_x1/cpu/dynarmic/cpu.hpp"

#include "hw/tegra_x1/cpu/dynarmic/mmu.hpp"
#include "hw/tegra_x1/cpu/dynarmic/thread.hpp"

namespace Hydra::HW::TegraX1::CPU::Dynarmic {

CPU::CPU() {
    // TODO

    // MMU
    mmu = new MMU();
}

CPU::~CPU() {
    // TODO
}

ThreadBase* CPU::CreateThread(MemoryBase* tls_mem) {
    Thread* thread = new Thread(mmu, tls_mem);
    // TODO

    return thread;
}

MMUBase* CPU::GetMMU() const { return mmu; }

} // namespace Hydra::HW::TegraX1::CPU::Dynarmic
