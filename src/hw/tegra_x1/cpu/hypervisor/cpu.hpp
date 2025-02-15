#pragma once

#include "hw/tegra_x1/cpu/cpu_base.hpp"
#include "hypervisor/const.hpp"
#include <Hypervisor/hv_vcpu_types.h>

namespace Hydra::HW::MMU {

class Memory;

}

namespace Hydra::HW::MMU::Hypervisor {

class MMU;

}

namespace Hydra::HW::CPU::Hypervisor {

class CPU : public CPUBase {
  public:
    CPU(HW::MMU::Hypervisor::MMU& mmu_);
    ~CPU();

    void Run();

    void AdvancePC();

    u64 GetRegX(u8 reg) const override {
        return GetReg((hv_reg_t)(HV_REG_X0 + reg));
    }
    void SetRegX(u8 reg, u64 value) override {
        SetReg((hv_reg_t)(HV_REG_X0 + reg), value);
    }

    void SetupVTimer();

    void UpdateVTimer();

    // Getters
    hv_vcpu_exit_t* GetExit() { return vcpu_exit; }

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
    void LogRegisters(u32 count = 31);

    void LogStackTrace(HW::MMU::Memory* stack_mem, uptr pc);

  private:
    HW::MMU::Hypervisor::MMU& mmu;

    hv_vcpu_t vcpu;
    hv_vcpu_exit_t* vcpu_exit;
};

} // namespace Hydra::HW::CPU::Hypervisor
