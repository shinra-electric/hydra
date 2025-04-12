#include "core/hw/tegra_x1/cpu/dynarmic/thread.hpp"

#include <mach/mach_time.h>
#include <dynarmic/interface/exclusive_monitor.h>

#include "core/hw/tegra_x1/cpu/dynarmic/mmu.hpp"

namespace Hydra::HW::TegraX1::CPU::Dynarmic {

Thread::Thread(MMU* mmu_, MemoryBase* tls_mem)
    : ThreadBase(tls_mem), mmu{mmu_} {
    exclusive_monitor = new Dyn::ExclusiveMonitor(4); // TODO: don't hardcode core count

    // TODO
}

Thread::~Thread() {
    delete jit;
}

void Thread::Configure(const std::function<bool(ThreadBase*, u64)>&
                           svc_handler_,
                       uptr tls_mem_base /*,
  uptr rom_mem_base*/, uptr stack_mem_end) {
    svc_handler = svc_handler_;
    tpidrro_el0 = tls_mem_base;

    // TODO: do this in the costructor

    DynA64::UserConfig config{};
    config.callbacks = this;

    // Multi-process state
    config.processor_id = 0;         // TODO: don't hardcode this
    config.global_monitor = exclusive_monitor;

    // System registers
    config.tpidrro_el0 = &tpidrro_el0;
    config.tpidr_el0 = nullptr; // TODO: what is this?
    config.dczid_el0 = 4;
    config.ctr_el0 = 0x8444c004;
    config.cntfrq_el0 = 19200000; // TODO: make this a constant

    // Unpredictable instructions
    config.define_unpredictable_behaviour = true;

    // Timing
    // TODO: correct?
    config.wall_clock_cntpct = true;
    config.enable_cycle_counting = false;

    // Code cache size
    config.code_cache_size = 128 * 1024 * 1024; // 128_MiB;

    jit = new DynA64::Jit(config);

    jit->SetSP(stack_mem_end);
}

void Thread::Run() { jit->Run(); }

void Thread::LogRegisters(bool simd, u32 count) {
    // TODO
}

void Thread::LogStackTrace() {
    u64 fp = jit->GetRegister(29);
    u64 lr = jit->GetRegister(30);
    u64 sp = jit->GetSP();

    LOG_DEBUG(CPU, "Stack trace:");
    // LOG_DEBUG(CPU, "SP: 0x{:08x}", sp);
    LOG_DEBUG(CPU, "0x{:08x}", jit->GetPC());

    for (uint64_t frame = 0; fp != 0; frame++) {
        LOG_DEBUG(CPU, "0x{:08x}", lr - 0x4);
        if (frame == MAX_STACK_TRACE_DEPTH - 1) {
            LOG_DEBUG(CPU, "... (more frames)");
            break;
        }

        // if (!stack_mem->AddrIsInRange(fp))
        //     break;
        // HACK
        if (fp < 0x10000000 || fp >= 0x20000000) {
            LOG_WARNING(Hypervisor, "Currputed stack");
            break;
        }

        u64 new_fp = mmu->Load<u64>(fp);
        lr = mmu->Load<u64>(fp + 8);

        fp = new_fp;
    }
}

u8 Thread::MemoryRead8(u64 addr) {
    return mmu->Load<u8>(addr);
}

u16 Thread::MemoryRead16(u64 addr) {
    return mmu->Load<u16>(addr);
}

u32 Thread::MemoryRead32(u64 addr) {
    return mmu->Load<u32>(addr);
}

u64 Thread::MemoryRead64(u64 addr) {
    return mmu->Load<u64>(addr);
}

DynA64::Vector Thread::MemoryRead128(u64 addr) {
    return mmu->Load<DynA64::Vector>(addr);
}

std::optional<u32> Thread::MemoryReadCode(u64 addr) {
    return mmu->Load<u32>(addr);
}

void Thread::MemoryWrite8(u64 addr, u8 value) {
    mmu->Store(addr, value);
}

void Thread::MemoryWrite16(u64 addr, u16 value) {
    mmu->Store(addr, value);
}

void Thread::MemoryWrite32(u64 addr, u32 value) {
    mmu->Store(addr, value);
}

void Thread::MemoryWrite64(u64 addr, u64 value) {
    mmu->Store(addr, value);
}

void Thread::MemoryWrite128(u64 addr, DynA64::Vector value) {
    mmu->Store(addr, value);
}

bool Thread::MemoryWriteExclusive8(u64 addr, u8 value, u8) {
    // TODO: exclusive
    mmu->Store(addr, value);
    return true;
}

bool Thread::MemoryWriteExclusive16(u64 addr, u16 value, u16) {
    // TODO: exclusive
    mmu->Store(addr, value);
    return true;
}

bool Thread::MemoryWriteExclusive32(u64 addr, u32 value, u32) {
    // TODO: exclusive
    mmu->Store(addr, value);
    return true;
}

bool Thread::MemoryWriteExclusive64(u64 addr, u64 value, u64 expected) {
    // TODO: exclusive
    mmu->Store(addr, value);
    return true;
}

bool Thread::MemoryWriteExclusive128(u64 addr, DynA64::Vector value,
                                     DynA64::Vector expected) {
    // TODO: exclusive
    mmu->Store(addr, value);
    return true;
}

u64 Thread::GetCNTPCT() {
    // TODO: use a cross-platform solution
    return mach_absolute_time();
}

} // namespace Hydra::HW::TegraX1::CPU::Dynarmic
