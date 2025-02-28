#include "hw/tegra_x1/cpu/hypervisor/thread.hpp"

#include "hw/tegra_x1/cpu/hypervisor/mmu.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"

#define MAX_STACK_TRACE_DEPTH 32

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

Thread::Thread(MMU* mmu_, CPU* cpu_) : mmu{mmu_}, cpu{cpu_} {
    // Create
    HYP_ASSERT_SUCCESS(hv_vcpu_create(&vcpu, &exit, NULL));

    // TODO: find out what this does
    SetReg(HV_REG_CPSR, 0x3c4);

    SetSysReg(HV_SYS_REG_MAIR_EL1, 0xfful);
    SetSysReg(HV_SYS_REG_TCR_EL1, 0x00000011B5193519ul);
    SetSysReg(HV_SYS_REG_SCTLR_EL1, 0x0000000034D5D925ul);

    // Enable FP and SIMD instructions.
    SetSysReg(HV_SYS_REG_CPACR_EL1, 0b11 << 20);

    // Trap debug access
    HYP_ASSERT_SUCCESS(hv_vcpu_set_trap_debug_exceptions(vcpu, true));
    // HYP_ASSERT_SUCCESS(hv_vcpu_set_trap_debug_reg_accesses(vcpu, true));
}

Thread::~Thread() { hv_vcpu_destroy(vcpu); }

void Thread::Configure(const std::function<bool(ThreadBase*, u64)>&
                           svc_handler_,
                       uptr kernel_mem_base,
                       uptr tls_mem_base /*,
  uptr rom_mem_base*/, uptr stack_mem_end, uptr exception_trampoline_base_) {
    svc_handler = svc_handler_;
    exception_trampoline_base = exception_trampoline_base_;

    // Trampoline
    SetSysReg(HV_SYS_REG_VBAR_EL1, kernel_mem_base);

    // Set the CPU's PC to execute from the trampoline
    // HYP_ASSERT_SUCCESS(
    //    hv_vcpu_set_reg(vcpu, HV_REG_PC, KERNEL_MEM_ADDR + 0x800));

    // TODO: what is this?
    SetSysReg(HV_SYS_REG_TTBR0_EL1, mmu->GetPageTablePa());
    // SetSysReg(HV_SYS_REG_TTBR1_EL1, mmu->GetKernelRangeMemory()->GetBase());

    // Initialize the stack pointer
    SetSysReg(HV_SYS_REG_SP_EL0, stack_mem_end);
    // TODO: set SP_EL1 as well?

    // Setup TLS pointer
    // TODO: offset by thread id * some alignment?
    SetSysReg(HV_SYS_REG_TPIDRRO_EL0, tls_mem_base);
}

void Thread::Run() {
    // Main run loop
    bool running = true;
    while (running) {
        HYP_ASSERT_SUCCESS(hv_vcpu_run(vcpu));

        if (exit->reason == HV_EXIT_REASON_EXCEPTION) {
            u64 syndrome = exit->exception.syndrome;
            u8 hvEc = (syndrome >> 26) & 0x3f;
            u64 pc = GetReg(HV_REG_PC);

            if (hvEc == 0x16) { // HVC
                u64 esr = GetSysReg(HV_SYS_REG_ESR_EL1);
                u8 ec = (esr >> 26) & 0x3f;

                u64 elr = GetSysReg(HV_SYS_REG_ELR_EL1);

                u64 far = GetSysReg(HV_SYS_REG_FAR_EL1);

                // u64 spsr = cpu->GetSysReg(HV_SYS_REG_SPSR_EL1);
                // u64 mode = (spsr >> 2) & 0x3;

                u32 instruction = mmu->Load<u32>(elr);

                switch (ec) {
                case 0x15:
                    running = svc_handler(this, esr & 0xffff);

                    // Debug
                    LogStackTrace(elr);
                    // cpu->LogRegisters();

                    break;
                case 0x18:
                    LOG_DEBUG(Hypervisor, "MSR MSR");
                    // TODO: implement
                    throw;
                    break;
                case 0x25: {
                    // Debug
                    // cpu->LogStackTrace(horizon.GetKernel().GetStackMemory(),
                    //                   elr);

                    // LOG_DEBUG(Hypervisor,
                    //           "Data abort (PC: 0x{:08x}, FAR: 0x{:08x}, "
                    //           "instruction: 0x{:08x})",
                    //           elr, far, instruction);

                    bool far_valid = (esr & 0x00000400) == 0;
                    ASSERT(far_valid, Hypervisor, "FAR not valid");

                    DataAbort(instruction, far, elr);

                    break;
                }
                default:
                    LOG_ERROR(
                        Hypervisor,
                        "Unknown HVC code (EC: 0x{:08x}, ESR: 0x{:08x}, PC: "
                        "0x{:08x}, FAR: "
                        "0x{:08x})",
                        ec, esr, GetSysReg(HV_SYS_REG_ELR_EL1),
                        GetSysReg(HV_SYS_REG_FAR_EL1));

                    // Debug
                    LogStackTrace(elr);

                    break;
                }

                // Set the PC to trampoline
                // TODO: most of the time we can skip msr, find out when
                SetReg(HV_REG_PC, exception_trampoline_base);
            } else if (hvEc == 0x17) { // SMC
                LOG_WARNING(Hypervisor, "SMC instruction");

                AdvancePC();
            } else if (hvEc == 0x18) {
                // TODO: this should not happen

                LOG_DEBUG(Hypervisor, "MSR MRS instruction");

                // Debug
                LogStackTrace(pc);

                // Manually execute the instruction
                u32 instruction = mmu->Load<u32>(pc);

                u8 opcode =
                    (instruction >> 24) & 0xFF; // Extract opcode (bits 31-24)
                u8 rt = instruction & 0x1F;     // Extract Rt (bits 4-0)

                u8 op0 = (instruction >> 19) & 0x3; // Extract op0 (bits 21-20)
                u8 op1 = (instruction >> 16) & 0x7; // Extract op1 (bits 18-16)
                u8 crn = (instruction >> 12) & 0xF; // Extract CRn (bits 15-12)
                u8 crm = (instruction >> 8) & 0xF;  // Extract CRm (bits 11-8)
                u8 op2 = (instruction >> 5) & 0x7;  // Extract op2 (bits 7-5)

                // std::cout << "Opcode: 0x" << std::hex << (int)opcode
                //           << std::endl;
                // std::cout << "First Operand (Rt): X" << std::dec << (int)rt
                //           << std::endl;
                // std::cout << "Second Operand (System Register): "
                //           << "op0=" << (int)op0 << ", op1=" << (int)op1
                //           << ", CRn=" << (int)crn << ", CRm=" << (int)crm
                //           << ", op2=" << (int)op2 << std::endl;

                SetReg((hv_reg_t)(HV_REG_X0 + rt), 0);

                // Set the return address
                // TODO: correct?
                // u64 elr = cpu->GetSysReg(HV_SYS_REG_ELR_EL1);
                // cpu->SetSysReg(HV_SYS_REG_ELR_EL1, elr + 4);
                AdvancePC();
            } else if (hvEc == 0x3C) { // BRK
                LOG_ERROR(Hypervisor, "BRK instruction");
                LogRegisters(true);
                break;
            } else {
                LOG_ERROR(
                    Hypervisor,
                    "Unexpected VM exception 0x{:08x} (EC: 0x{:08x}, ESR: "
                    "0x{:08x}, "
                    "VirtAddr: "
                    "0x{:08x}, IPA: 0x{:08x}, PC: 0x{:08x}, ELR: 0x{:08x}, "
                    "instruction: "
                    "0x{:08x})",
                    syndrome, hvEc, GetSysReg(HV_SYS_REG_ESR_EL1),
                    exit->exception.virtual_address,
                    exit->exception.physical_address, pc,
                    GetSysReg(HV_SYS_REG_ELR_EL1), mmu->Load<u32>(pc));

                // Debug
                LogStackTrace(pc);
                LogRegisters();

                break;
            }
        } else if (exit->reason == HV_EXIT_REASON_VTIMER_ACTIVATED) {
            UpdateVTimer();
            LOG_DEBUG(Hypervisor, "VTimer");
        } else {
            // TODO: don't cast to u32
            LOG_ERROR(Hypervisor, "Unexpected VM exit reason {}",
                      (u32)exit->reason);
            break;
        }
    }
}

void Thread::AdvancePC() {
    u64 pc = GetReg(HV_REG_PC);
    SetReg(HV_REG_PC, pc + 4);
}

void Thread::SetupVTimer() {
    SetSysReg(HV_SYS_REG_CNTV_CTL_EL0, 1);
    SetSysReg(HV_SYS_REG_CNTV_CVAL_EL0,
              1000000000000000000); // TODO: set to current time
}

void Thread::UpdateVTimer() {
    SetupVTimer();
    hv_vcpu_set_vtimer_mask(vcpu, false);
}

void Thread::LogRegisters(bool simd, u32 count) {
    LOG_DEBUG(CPU, "Reg dump:");
    for (u32 i = 0; i < count; i++) {
        LOG_DEBUG(CPU, "X{}: 0x{:08x}", i, GetRegX(i));
    }
    if (simd) {
        for (u32 i = 0; i < count; i++) {
            auto reg = GetRegQ(i);
            LOG_DEBUG(CPU, "Q{}: 0x{:08x}{:08x}", i, *(u64*)&reg,
                      *((u64*)&reg + 1)); // TODO: correct?
        }
    }
    LOG_DEBUG(CPU, "SP: 0x{:08x}", GetSysReg(HV_SYS_REG_SP_EL0));
}

void Thread::LogStackTrace(uptr pc) {
    u64 fp = GetReg(HV_REG_FP);
    u64 lr = GetReg(HV_REG_LR);
    u64 sp = GetSysReg(HV_SYS_REG_SP_EL0);

    LOG_DEBUG(CPU, "Stack trace:");
    // LOG_DEBUG(CPU, "SP: 0x{:08x}", sp);
    LOG_DEBUG(CPU, "0x{:08x}", pc);

    for (uint64_t frame = 0; fp != 0; frame++) {
        LOG_DEBUG(CPU, "0x{:08x}", lr - 0x4);
        if (frame == MAX_STACK_TRACE_DEPTH - 1) {
            LOG_DEBUG(CPU, "... (more frames)");
            break;
        }

        // if (!stack_mem->AddrIsInRange(fp))
        //     break;

        u64 new_fp = mmu->Load<u64>(fp);
        lr = mmu->Load<u64>(fp + 8);

        fp = new_fp;
    }
}

void Thread::DataAbort(u32 instruction, u64 far, u64 elr) {
    LOG_WARNING(Hypervisor,
                "PC: 0x{:08x}, instruction: 0x{:08x}, FAR: 0x{:08x} ", elr,
                instruction, far);

    // Set the return address
    // TODO: correct?
    SetSysReg(HV_SYS_REG_ELR_EL1, elr + 4);
}

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
