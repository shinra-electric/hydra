#pragma once

#include "core/hw/tegra_x1/cpu/const.hpp"

namespace hydra::hw::tegra_x1::cpu {

class IMemory;
class IMmu;
class IThread;

typedef std::function<void(IThread*, u64)> svc_handler_fn_t;
typedef std::function<bool()> stop_requested_fn_t;
typedef std::function<void(vaddr_t)> stack_frame_callback_fn_t;

struct ThreadState {
    u64 r[29];
    u64 fp;
    u64 lr;
    u64 sp;
    u64 pc;
    u64 pstate; // TODO: what is this?
    u128 v[32];
    u32 fpcr;
    u32 fpsr;
};

class IThread {
  public:
    IThread(IMmu* mmu_, const svc_handler_fn_t& svc_handler_,
            const stop_requested_fn_t& stop_requested_, IMemory* tls_mem_)
        : mmu{mmu_}, svc_handler{svc_handler_},
          stop_requested{stop_requested_}, tls_mem{tls_mem_} {}
    virtual ~IThread() {}

    virtual void Run() = 0;

    // Debug
    virtual void LogRegisters(bool simd = false, u32 count = 32) = 0;
    void GetStackTrace(stack_frame_callback_fn_t callback);

    // Getters
    IMemory* GetTlsMemory() const { return tls_mem; }

  protected:
    IMmu* mmu;
    svc_handler_fn_t svc_handler;
    stop_requested_fn_t stop_requested;
    IMemory* tls_mem;

    ThreadState state{};

  public:
    REF_GETTER(state, GetState);
};

} // namespace hydra::hw::tegra_x1::cpu
