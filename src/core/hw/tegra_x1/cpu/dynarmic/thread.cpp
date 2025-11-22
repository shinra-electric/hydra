#include "core/hw/tegra_x1/cpu/dynarmic/thread.hpp"

#include <dynarmic/interface/exclusive_monitor.h>
#include <mach/mach_time.h>

#include "core/hw/tegra_x1/cpu/dynarmic/mmu.hpp"
#include "dynarmic/interface/optimization_flags.h"

#define MMU static_cast<Mmu*>(mmu)

namespace hydra::hw::tegra_x1::cpu::dynarmic {

// HACK
static Dynarmic::ExclusiveMonitor
    g_exclusive_monitor(4); // TODO: don't hardcode core count

Thread::Thread(IMmu* mmu, const ThreadCallbacks& callbacks, IMemory* tls_mem,
               vaddr_t tls_mem_base)
    : IThread(mmu, callbacks, tls_mem) {
    tpidrro_el0 = tls_mem_base;

    // Create JIT
    Dynarmic::A64::UserConfig config{};
    config.callbacks = this;

    // Multi-process state
    config.processor_id = 0; // TODO: don't hardcode this
    config.global_monitor = &g_exclusive_monitor;

    // System registers
    config.tpidrro_el0 = &tpidrro_el0;
    config.tpidr_el0 = nullptr; // TODO: what is this?
    config.dczid_el0 = 4;
    config.ctr_el0 = 0x8444c004;
    config.cntfrq_el0 = CLOCK_RATE_HZ;

    // Unpredictable instructions
    config.define_unpredictable_behaviour = true;

    // Timing
    // TODO: correct?
    config.wall_clock_cntpct = true;
    config.enable_cycle_counting = false;

    // Code cache size
    config.code_cache_size = 128 * 1024 * 1024; // 128_MiB;

    // TODO: make this configurable
    // config.optimizations = Dyn::no_optimizations;

    jit = new Dynarmic::A64::Jit(config);
}

Thread::~Thread() { delete jit; }

void Thread::Run() {
    DeserializeState();
    jit->Run();
}

u8 Thread::MemoryRead8(u64 addr) { return MMU->Read<u8>(addr); }

u16 Thread::MemoryRead16(u64 addr) { return MMU->Read<u16>(addr); }

u32 Thread::MemoryRead32(u64 addr) { return MMU->Read<u32>(addr); }

u64 Thread::MemoryRead64(u64 addr) { return MMU->Read<u64>(addr); }

Dynarmic::A64::Vector Thread::MemoryRead128(u64 addr) {
    return MMU->Read<Dynarmic::A64::Vector>(addr);
}

std::optional<u32> Thread::MemoryReadCode(u64 addr) {
    return MMU->Read<u32>(addr);
}

void Thread::MemoryWrite8(u64 addr, u8 value) { MMU->Write(addr, value); }

void Thread::MemoryWrite16(u64 addr, u16 value) { MMU->Write(addr, value); }

void Thread::MemoryWrite32(u64 addr, u32 value) { MMU->Write(addr, value); }

void Thread::MemoryWrite64(u64 addr, u64 value) { MMU->Write(addr, value); }

void Thread::MemoryWrite128(u64 addr, Dynarmic::A64::Vector value) {
    MMU->Write(addr, value);
}

bool Thread::MemoryWriteExclusive8(u64 addr, u8 value, u8) {
    MMU->WriteExclusive(addr, value);
    return true;
}

bool Thread::MemoryWriteExclusive16(u64 addr, u16 value, u16) {
    MMU->WriteExclusive(addr, value);
    return true;
}

bool Thread::MemoryWriteExclusive32(u64 addr, u32 value, u32) {
    MMU->WriteExclusive(addr, value);
    return true;
}

bool Thread::MemoryWriteExclusive64(u64 addr, u64 value, u64 expected) {
    MMU->WriteExclusive(addr, value);
    return true;
}

bool Thread::MemoryWriteExclusive128(u64 addr, Dynarmic::A64::Vector value,
                                     Dynarmic::A64::Vector expected) {
    MMU->WriteExclusive(addr, value);
    return true;
}

void Thread::CallSVC(u32 svc) {
    SerializeState();
    callbacks.svc_handler(this, svc);
    CheckForStopRequest();
    DeserializeState();
}

void Thread::ExceptionRaised(u64 pc, Dynarmic::A64::Exception exception) {
    // TODO: handle the exception
    // TODO: debugger break
    LOG_FATAL(Dynarmic, "Exception");
}

u64 Thread::GetCNTPCT() { return get_absolute_time(); }

void Thread::SerializeState() {
    for (u32 i = 0; i < 29; i++)
        state.r[i] = jit->GetRegister(i);
    state.fp = jit->GetRegister(29);
    state.lr = jit->GetRegister(30);
    state.sp = jit->GetSP();
    state.pc = jit->GetPC();
    state.pstate = jit->GetPstate();
    for (u32 i = 0; i < 32; i++)
        state.v[i] = std::bit_cast<u128>(jit->GetVector(i)); // TODO: correct?
    state.fpcr = jit->GetFpcr();
    state.fpsr = jit->GetFpsr();
}

void Thread::DeserializeState() {
    for (u32 i = 0; i < 29; i++)
        jit->SetRegister(i, state.r[i]);
    jit->SetRegister(29, state.fp);
    jit->SetRegister(30, state.lr);
    jit->SetSP(state.sp);
    jit->SetPC(state.pc);
    jit->SetPstate(state.pstate);
    for (u32 i = 0; i < 32; i++)
        jit->SetVector(i, std::bit_cast<Dynarmic::A64::Vector>(
                              state.v[i])); // TODO: correct?
    jit->SetFpcr(state.fpcr);
    jit->SetFpsr(state.fpsr);
}

} // namespace hydra::hw::tegra_x1::cpu::dynarmic
