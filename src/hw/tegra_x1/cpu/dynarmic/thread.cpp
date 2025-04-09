#include "hw/tegra_x1/cpu/dynarmic/thread.hpp"

#include "hw/tegra_x1/cpu/dynarmic/mmu.hpp"

namespace Hydra::HW::TegraX1::CPU::Dynarmic {

Thread::Thread(MMU* mmu_, MemoryBase* tls_mem)
    : ThreadBase(tls_mem), mmu{mmu_} {
    // TODO
}

Thread::~Thread() {
    // TODO
}

void Thread::Configure(const std::function<bool(ThreadBase*, u64)>&
                           svc_handler_,
                       uptr tls_mem_base /*,
  uptr rom_mem_base*/, uptr stack_mem_end) {
    svc_handler = svc_handler_;

    // TODO
}

void Thread::Run() {
    // TODO
}

void Thread::LogRegisters(bool simd, u32 count) {
    // TODO
}

} // namespace Hydra::HW::TegraX1::CPU::Dynarmic
