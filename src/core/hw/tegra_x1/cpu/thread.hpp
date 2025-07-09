#pragma once

#include "core/hw/tegra_x1/cpu/const.hpp"

namespace hydra::hw::tegra_x1::cpu {

class IMemory;
class IMmu;

typedef std::function<void(vaddr_t)> stack_frame_callback_fn_t;

class IThread {
  public:
    IThread(IMmu* mmu_, IMemory* tls_mem_) : mmu{mmu_}, tls_mem{tls_mem_} {}
    virtual ~IThread() {}

    virtual void
    Initialize(const std::function<bool(IThread*, u64)>& svc_handler,
               uptr tls_mem_base, uptr stack_mem_end) = 0;

    virtual void Run() = 0;

    virtual u64 GetRegX(u8 reg) const = 0;
    virtual void SetRegX(u8 reg, u64 value) = 0;
    virtual u64 GetPC() = 0;
    virtual void SetPC(u64 value) = 0;
    virtual u64 GetFP() = 0;
    virtual u64 GetLR() = 0;
    virtual u64 GetSP() = 0;
    virtual u64 GetElr() = 0;

    u32 GetRegW(u8 reg) const { return static_cast<u32>(GetRegX(reg)); }
    void SetRegW(u8 reg, u32 value) { SetRegX(reg, static_cast<u64>(value)); }

    // Debug
    virtual void LogRegisters(bool simd = false, u32 count = 32) = 0;
    void GetStackTrace(stack_frame_callback_fn_t callback);

    // Getters
    IMemory* GetTlsMemory() const { return tls_mem; }

  protected:
    IMmu* mmu;
    IMemory* tls_mem;
};

} // namespace hydra::hw::tegra_x1::cpu
