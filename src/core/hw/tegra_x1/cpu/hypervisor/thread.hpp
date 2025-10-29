#pragma once

#include "core/hw/tegra_x1/cpu/hypervisor/const.hpp"
#include "core/hw/tegra_x1/cpu/thread.hpp"

namespace hydra::horizon {
class OS;
}

namespace hydra::hw::tegra_x1::cpu {
class IMemory;
}

namespace hydra::hw::tegra_x1::cpu::hypervisor {

class Cpu;
class Mmu;

class Thread : public IThread {
  public:
    Thread(IMmu* mmu, const svc_handler_fn_t& svc_handler,
           const stop_requested_fn_t& stop_requested, IMemory* tls_mem,
           vaddr_t tls_mem_base, vaddr_t stack_mem_end);
    ~Thread() override;

    void Run() override;

    void SetupVTimer();

    void UpdateVTimer();

    // Debug
    void LogRegisters(bool simd = false, u32 count = 32) override;

  private:
    hv_vcpu_t vcpu;
    hv_vcpu_exit_t* exit;
    bool exception{false};

    u64 interrupt_time_delta_ticks;

    // State
    void SerializeState();
    void DeserializeState();

    void InstructionTrap(u32 esr);
    void DataAbort(u32 instruction, u64 far, u64 elr);

    // Helpers
    u64 GetReg(hv_reg_t reg) const {
        u64 value;
        HV_ASSERT_SUCCESS(hv_vcpu_get_reg(vcpu, reg, &value));

        return value;
    }

    void SetReg(hv_reg_t reg, u64 value) {
        HV_ASSERT_SUCCESS(hv_vcpu_set_reg(vcpu, reg, value));
    }

    u128 GetSimdFpReg(u8 reg) const {
        hv_simd_fp_uchar16_t value;
        HV_ASSERT_SUCCESS(hv_vcpu_get_simd_fp_reg(
            vcpu, (hv_simd_fp_reg_t)(HV_SIMD_FP_REG_Q0 + reg), &value));

        // TODO: correct?
        return std::bit_cast<u128>(value);
    }

    void SetSimdFpReg(u8 reg, u128 value) {
        // TODO: correct?
        HV_ASSERT_SUCCESS(hv_vcpu_set_simd_fp_reg(
            vcpu, (hv_simd_fp_reg_t)(HV_SIMD_FP_REG_Q0 + reg),
            std::bit_cast<hv_simd_fp_uchar16_t>(value)));
    }

    u64 GetSysReg(hv_sys_reg_t reg) const {
        u64 value;
        HV_ASSERT_SUCCESS(hv_vcpu_get_sys_reg(vcpu, reg, &value));

        return value;
    }

    void SetSysReg(hv_sys_reg_t reg, u64 value) {
        HV_ASSERT_SUCCESS(hv_vcpu_set_sys_reg(vcpu, reg, value));
    }
};

} // namespace hydra::hw::tegra_x1::cpu::hypervisor
