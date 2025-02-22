#pragma once

#include "hw/tegra_x1/cpu/hypervisor/const.hpp"
#include "hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra::Horizon {
class OS;
}

namespace Hydra::HW::TegraX1::CPU {
class Memory;
}

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

class MMU;
class CPU;

class Thread : public ThreadBase {
  public:
    Thread(MMU* mmu_, CPU* cpu_);
    ~Thread() override;

    void Configure(const std::function<bool(ThreadBase*, u64)>& svc_handler_,
                   uptr kernel_mem_base,
                   uptr tls_mem_base /*, uptr rom_mem_base*/,
                   uptr stack_mem_end,
                   uptr exception_trampoline_base_) override;

    void Run() override;

    void AdvancePC();

    u64 GetRegX(u8 reg) const override {
        return GetReg((hv_reg_t)(HV_REG_X0 + reg));
    }

    void SetRegX(u8 reg, u64 value) override {
        SetReg((hv_reg_t)(HV_REG_X0 + reg), value);
    }

    void SetRegPC(u64 value) override { SetReg(HV_REG_PC, value); }

    void SetupVTimer();

    void UpdateVTimer();

    // Getters
    u64 GetReg(hv_reg_t reg) const {
        u64 value;
        HYP_ASSERT_SUCCESS(hv_vcpu_get_reg(vcpu, reg, &value));

        return value;
    }

    hv_simd_fp_uchar16_t GetRegQ(u8 reg) const {
        hv_simd_fp_uchar16_t value;
        HYP_ASSERT_SUCCESS(hv_vcpu_get_simd_fp_reg(
            vcpu, (hv_simd_fp_reg_t)(HV_SIMD_FP_REG_Q0 + reg), &value));

        return value;
    }

    u64 GetSysReg(hv_sys_reg_t reg) const {
        u64 value;
        HYP_ASSERT_SUCCESS(hv_vcpu_get_sys_reg(vcpu, reg, &value));

        return value;
    }

    // Setters
    void SetReg(hv_reg_t reg, u64 value) {
        HYP_ASSERT_SUCCESS(hv_vcpu_set_reg(vcpu, reg, value));
    }

    void SetRegQ(u8 reg, hv_simd_fp_uchar16_t value) {
        HYP_ASSERT_SUCCESS(hv_vcpu_set_simd_fp_reg(
            vcpu, (hv_simd_fp_reg_t)(HV_SIMD_FP_REG_Q0 + reg), value));
    }

    void SetSysReg(hv_sys_reg_t reg, u64 value) {
        HYP_ASSERT_SUCCESS(hv_vcpu_set_sys_reg(vcpu, reg, value));
    }

    // Debug
    void LogRegisters(bool simd = false, u32 count = 32);

    void LogStackTrace(uptr pc);

  private:
    MMU* mmu;
    CPU* cpu;

    std::function<bool(ThreadBase*, u64)> svc_handler;
    uptr exception_trampoline_base;

    hv_vcpu_t vcpu;
    hv_vcpu_exit_t* exit;

    void DataAbort(u32 instruction, u64 far, u64 elr);

    // Interpreter
    void InterpretLDAXR(u8 size0, u8 out_reg, u64 addr);
    void InterpretSTLXR(u8 size0, u8 out_res_reg, u8 reg, u64 addr);
    void InterpretDC(u64 addr);
    void InterpretLDR(u8 size0, u8 size1, u8 out_reg, u64 addr);
    void InterpretSTR(u8 size0, u8 size1, u8 reg, u64 addr);
    void InterpretLDP(u8 size0, u8 size1, u8 out_reg0, u8 out_reg1, u64 addr);
    void InterpretSTP(u8 size0, u8 size1, u8 reg0, u8 reg1, u64 addr);
};

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
