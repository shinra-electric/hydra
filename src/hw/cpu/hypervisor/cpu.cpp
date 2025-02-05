#include "hw/cpu/hypervisor/cpu.hpp"

#include "common.hpp"
#include "hw/mmu/hypervisor/mmu.hpp"
#include "hw/mmu/memory.hpp"

namespace Hydra::HW::CPU::Hypervisor {

CPU::CPU(HW::MMU::Hypervisor::MMU& mmu_) : mmu{mmu_} {
    // Create
    HYP_ASSERT_SUCCESS(hv_vcpu_create(&vcpu, &vcpu_exit, NULL));

    // TODO: what are these?
    // SetSysReg(HV_SYS_REG_TCR_EL1, 0x00000011B5193519UL);
    // SetSysReg(HV_SYS_REG_SCTLR_EL1, 0x0000000034D5D925UL);

    // Enable FP and SIMD instructions.
    // TODO: find out how this works
    SetSysReg(HV_SYS_REG_CPACR_EL1, 0b11 << 20);

    SetSysReg(HV_SYS_REG_MAIR_EL1, 0xffUL);

    // Trap debug access (BRK)
    HYP_ASSERT_SUCCESS(hv_vcpu_set_trap_debug_exceptions(vcpu, true));
}

CPU::~CPU() { hv_vcpu_destroy(vcpu); }

void CPU::Run() { HYP_ASSERT_SUCCESS(hv_vcpu_run(vcpu)); }

void CPU::AdvancePC() {
    u64 pc = GetReg(HV_REG_PC);
    SetReg(HV_REG_PC, pc + 4);
}

u64 CPU::GetReg(hv_reg_t reg) const {
    u64 value;
    HYP_ASSERT_SUCCESS(hv_vcpu_get_reg(vcpu, reg, &value));

    return value;
}

u64 CPU::GetSysReg(hv_sys_reg_t sys_reg) const {
    u64 value;
    HYP_ASSERT_SUCCESS(hv_vcpu_get_sys_reg(vcpu, sys_reg, &value));

    return value;
}

void CPU::SetReg(hv_reg_t reg, u64 value) {
    HYP_ASSERT_SUCCESS(hv_vcpu_set_reg(vcpu, reg, value));
}

void CPU::SetSysReg(hv_sys_reg_t sys_reg, u64 value) {
    HYP_ASSERT_SUCCESS(hv_vcpu_set_sys_reg(vcpu, sys_reg, value));
}

void CPU::LogRegisters(u32 count) {
    printf("Reg dump:\n");
    for (u32 i = 0; i < count; i++) {
        printf("X%u: 0x%llx\n", i, GetReg((hv_reg_t)(HV_REG_X0 + i)));
    }
}

void CPU::LogStackTrace(HW::MMU::Memory* stack_mem) {
    u64 fp = GetReg(HV_REG_FP);
    u64 lr = GetReg(HV_REG_LR);
    u64 sp = GetSysReg(HV_SYS_REG_SP_EL0);

    printf("Stack trace:\n");
    printf("SP: 0x%08llx\n", sp);

    for (uint64_t frame = 0; frame < 32 && fp != 0; frame++) {
        printf("LR = 0x%llx\n", lr);

        uint64_t saved_fp, saved_lr;

        u64 new_fp = *((u64*)stack_mem->UnmapPtr(fp));
        lr = *((u64*)stack_mem->UnmapPtr(fp + 8));

        fp = new_fp;
    }
}

} // namespace Hydra::HW::CPU::Hypervisor
