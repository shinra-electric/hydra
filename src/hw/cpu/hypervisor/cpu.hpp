#pragma once

#include "hw/cpu/cpu.hpp"
#include "hypervisor/const.hpp"

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

    // Getters
    hv_vcpu_exit_t* GetExit() { return vcpu_exit; }

    u64 GetReg(hv_reg_t reg) const;

    u64 GetSysReg(hv_sys_reg_t reg) const;

    // Setters
    void SetReg(hv_reg_t reg, u64 value);

    void SetSysReg(hv_sys_reg_t sys_reg, u64 value);

    // Debug
    void LogRegisters(u32 count = 31);

    void LogStackTrace(HW::MMU::Memory* stack_mem);

  private:
    HW::MMU::Hypervisor::MMU& mmu;

    hv_vcpu_t vcpu;
    hv_vcpu_exit_t* vcpu_exit;
};

} // namespace Hydra::HW::CPU::Hypervisor
