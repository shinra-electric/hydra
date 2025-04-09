#pragma once

#include "hw/tegra_x1/cpu/dynarmic/const.hpp"
#include "hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra::Horizon {
class OS;
}

namespace Hydra::HW::TegraX1::CPU {
class Memory;
}

namespace Hydra::HW::TegraX1::CPU::Dynarmic {

class MMU;
class CPU;

class Thread : public ThreadBase {
  public:
    Thread(MMU* mmu_, MemoryBase* tls_mem);
    ~Thread() override;

    void Configure(const std::function<bool(ThreadBase*, u64)>& svc_handler_,
                   uptr tls_mem_base /*, uptr rom_mem_base*/,
                   uptr stack_mem_end) override;

    void Run() override;

    u64 GetRegX(u8 reg) const override {
        // TODO
        return invalid<u64>();
    }

    void SetRegX(u8 reg, u64 value) override {
        // TODO
    }

    void SetRegPC(u64 value) override {
        // TODO
    }

    // Debug
    void LogRegisters(bool simd = false, u32 count = 32) override;

  private:
    MMU* mmu;

    std::function<bool(ThreadBase*, u64)> svc_handler;
};

} // namespace Hydra::HW::TegraX1::CPU::Dynarmic
