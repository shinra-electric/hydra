#include "hypervisor/hypervisor.hpp"

#include "common.hpp"
#include "horizon/horizon.hpp"
#include "hw/cpu/hypervisor/cpu.hpp"
#include "hw/mmu/hypervisor/mmu.hpp"
#include "hw/mmu/memory.hpp"

namespace Hydra::Hypervisor {

Hypervisor::Hypervisor(Horizon::OS& horizon_) : horizon{horizon_} {
    // Create VM
    hv_vm_config_t vm_config = hv_vm_config_create();
    // hv_vm_config_set_el2_enabled(config, true);
    HYP_ASSERT_SUCCESS(hv_vm_create(vm_config));

    // Create GIC
    // hv_gic_config_t gic_config = hv_gic_config_create();
    // hv_gic_config_set_msi_region_base(&gic_config, TODO);
    // hv_gic_config_set_msi_interrupt_range(&gic_config, TODO);
    // HYP_ASSERT_SUCCESS(hv_gic_create(gic_config));

    // MMU
    mmu = new HW::MMU::Hypervisor::MMU();
    horizon.SetMMU(mmu);

    mmu->MapMemory(horizon.GetKernel().GetStackMemory());
    mmu->MapMemory(horizon.GetKernel().GetKernelMemory());
    mmu->MapMemory(horizon.GetKernel().GetTlsMemory());

    // CPU
    cpu = new HW::CPU::Hypervisor::CPU(*mmu);

    // Configure timers
    cpu->SetupVTimer();
}

Hypervisor::~Hypervisor() {
    delete cpu;

    hv_vm_destroy();
}

void Hypervisor::LoadROM(Rom* rom) {
    horizon.GetKernel().LoadROM(rom);

    // Map ROM
    mmu->MapMemory(horizon.GetKernel().GetRomMemory());
    mmu->MapMemory(horizon.GetKernel().GetBssMemory());

    // Set initial PC
    cpu->SetReg(HV_REG_PC, horizon.GetKernel().GetRomMemory()->GetBase() +
                               rom->GetTextOffset());

    // Set arguments

    // From https://github.com/switchbrew/libnx

    // NSO
    // TODO

    // NRO
    // TODO: should be ptr to env context
    cpu->SetReg(HV_REG_X0, 0);
    cpu->SetReg(HV_REG_X1, 0xFFFFFFFFFFFFFFFF);

    // User-mode exception entry
    // TODO: what is this?

    // Trampoline
    cpu->SetSysReg(HV_SYS_REG_VBAR_EL1,
                   horizon.GetKernel().GetKernelMemory()->GetBase());

    // Set the CPU's PC to execute from the trampoline
    // HYP_ASSERT_SUCCESS(
    //    hv_vcpu_set_reg(vcpu, HV_REG_PC, KERNEL_MEM_ADDR + 0x800));

    // Explicitly set CPSR
    cpu->SetReg(HV_REG_CPSR, 0x3c4);

    // TODO: correct?
    cpu->SetSysReg(HV_SYS_REG_TTBR0_EL1,
                   /*_userRange.GetIpaBase()*/ horizon.GetKernel()
                       .GetRomMemory()
                       ->GetBase());
    cpu->SetSysReg(HV_SYS_REG_TTBR1_EL1,
                   /*_kernelRange.GetIpaBase()*/ horizon.GetKernel()
                       .GetKernelMemory()
                       ->GetBase());

    // Initialize the stack pointer
    auto stack_mem = horizon.GetKernel().GetStackMemory();
    cpu->SetSysReg(HV_SYS_REG_SP_EL0,
                   stack_mem->GetBase() + stack_mem->GetSize() / 2);
    cpu->SetSysReg(HV_SYS_REG_SP_EL1,
                   stack_mem->GetBase() + stack_mem->GetSize());

    // Setup TLS pointer
    // TODO: offset by thread id * some alignment
    cpu->SetSysReg(HV_SYS_REG_TPIDRRO_EL0,
                   horizon.GetKernel().GetTlsMemory()->GetBase());
}

void Hypervisor::Run() {
    // Main run loop
    bool running = true;
    while (running) {
        cpu->Run();

        auto exit = cpu->GetExit();
        if (exit->reason == HV_EXIT_REASON_EXCEPTION) {
            u64 syndrome = exit->exception.syndrome;
            u8 hvEc = (syndrome >> 26) & 0x3f;

            if (hvEc == 0x16) { // HVC
                // u64 x0;
                // HYP_ASSERT_SUCCESS(hv_vcpu_get_reg(vcpu, HV_REG_X0, &x0));
                // printf("VM made an HVC call! x0 register holds 0x%llx\n",
                // x0);

                u64 esr = cpu->GetSysReg(HV_SYS_REG_ESR_EL1);
                u8 ec = (esr >> 26) & 0x3f;

                u64 elr = cpu->GetSysReg(HV_SYS_REG_ELR_EL1);

                // u64 spsr = cpu->GetSysReg(HV_SYS_REG_SPSR_EL1);
                // u64 mode = (spsr >> 2) & 0x3;

                switch (ec) {
                case 0x15:
                    // Debug
                    cpu->LogStackTrace(horizon.GetKernel().GetStackMemory());
                    // cpu->LogRegisters();

                    running =
                        horizon.GetKernel().SupervisorCall(cpu, esr & 0xffff);

                    // HACK
                    // cpu->SetSysReg(HV_SYS_REG_SPSR_EL1,
                    //               cpu->GetSysReg(HV_SYS_REG_SPSR_EL1) &
                    //               ~0x1);

                    break;
                case 0x18:
                    printf("MSR MSR\n");
                    // TODO: implement
                    throw;
                    break;
                case 0x25:
                    // Debug
                    cpu->LogStackTrace(horizon.GetKernel().GetStackMemory());

                    // TODO: check if valid
                    printf("Data abort (PC 0x%08llx)\n", elr);

                    // printf("X3: 0x%08llx\n", cpu->GetReg(HV_REG_X3));

                    // HACK: write the result code to a register
                    cpu->SetReg(
                        (hv_reg_t)(HV_REG_X0 +
                                   EXTRACT_BITS(*((u32*)mmu->UnmapPtr(
                                                    cpu->GetReg(HV_REG_PC))),
                                                4, 0)),
                        0);

                    // Set the return address
                    // TODO: correct?
                    cpu->SetSysReg(HV_SYS_REG_ELR_EL1, elr + 4);
                    break;
                default:
                    // Debug
                    cpu->LogStackTrace(horizon.GetKernel().GetStackMemory());

                    printf("Unknown HVC code (EC 0x%08x, ESR 0x%08llx, PC "
                           "0x%08llx, FAR "
                           "0x%08llx)\n",
                           ec, esr, cpu->GetSysReg(HV_SYS_REG_ELR_EL1),
                           cpu->GetSysReg(HV_SYS_REG_FAR_EL1));
                    // printf("X3: 0x%08llx\n", cpu->GetReg(HV_REG_X3));
                    break;
                }

                // Set the PC to trampoline
                // TODO: most of the time we can skip msr, find out when
                cpu->SetReg(HV_REG_PC,
                            horizon.GetKernel().GetKernelMemory()->GetBase() +
                                0x800);
            } else if (hvEc == 0x17) { // SMC
                // uint64_t x0;
                // HYP_ASSERT_SUCCESS(hv_vcpu_get_reg(vcpu, HV_REG_X0, &x0));
                // printf("VM made an SMC call! x0 register holds 0x%llx\n",
                // x0); printf("Return to get on next instruction.\n");
                printf("SMC instruction\n");

                cpu->AdvancePC();
            } else if (hvEc == 0x18) {
                // TODO: this should not happen

                // Debug
                cpu->LogStackTrace(horizon.GetKernel().GetStackMemory());

                printf("MSR MRS instruction\n");

                // Manually execute the instruction
                u32 instruction =
                    *((u32*)mmu->UnmapPtr(cpu->GetReg(HV_REG_PC)));

                u8 opcode =
                    (instruction >> 24) & 0xFF; // Extract opcode (bits 31-24)
                u8 rt = instruction & 0x1F;     // Extract Rt (bits 4-0)

                u8 op0 = (instruction >> 19) & 0x3; // Extract op0 (bits 21-20)
                u8 op1 = (instruction >> 16) & 0x7; // Extract op1 (bits 18-16)
                u8 crn = (instruction >> 12) & 0xF; // Extract CRn (bits 15-12)
                u8 crm = (instruction >> 8) & 0xF;  // Extract CRm (bits 11-8)
                u8 op2 = (instruction >> 5) & 0x7;  // Extract op2 (bits 7-5)

                std::cout << "Opcode: 0x" << std::hex << (int)opcode
                          << std::endl;
                std::cout << "First Operand (Rt): X" << std::dec << (int)rt
                          << std::endl;
                std::cout << "Second Operand (System Register): "
                          << "op0=" << (int)op0 << ", op1=" << (int)op1
                          << ", CRn=" << (int)crn << ", CRm=" << (int)crm
                          << ", op2=" << (int)op2 << std::endl;

                cpu->SetReg((hv_reg_t)(HV_REG_X0 + rt), 0);

                // Set the return address
                // TODO: correct?
                // u64 elr = cpu->GetSysReg(HV_SYS_REG_ELR_EL1);
                // cpu->SetSysReg(HV_SYS_REG_ELR_EL1, elr + 4);
                cpu->AdvancePC();
            } else if (hvEc == 0x3C) { // BRK
                printf("BRK instruction\n");
                cpu->LogRegisters(5);
                break;
            } else {
                // Debug
                cpu->LogStackTrace(horizon.GetKernel().GetStackMemory());
                cpu->LogRegisters();

                fprintf(stderr,
                        "Unexpected VM exception: 0x%llx, EC 0x%x, ESR 0x%llx, "
                        "VirtAddr "
                        "0x%llx, IPA 0x%llx, PC 0x%llx, FAR 0x%llx\n",
                        syndrome, hvEc, cpu->GetSysReg(HV_SYS_REG_ESR_EL1),
                        exit->exception.virtual_address,
                        exit->exception.physical_address,
                        cpu->GetReg(HV_REG_PC),
                        cpu->GetSysReg(HV_SYS_REG_FAR_EL1));
                // printf("X2: 0x%08llx\n", cpu->GetReg(HV_REG_X2));
                //  printf("INSTRUCTION: 0x%08x\n",
                //        *((u32*)horizon.GetKernel().UnmapPtr(
                //            cpu->GetReg(HV_REG_PC))));
                break;
            }
        } else if (exit->reason == HV_EXIT_REASON_VTIMER_ACTIVATED) {
            cpu->UpdateVTimer();
            printf("VTimer\n");
        } else {
            fprintf(stderr, "Unexpected VM exit reason: %d\n", exit->reason);
            break;
        }
    }
}

} // namespace Hydra::Hypervisor
