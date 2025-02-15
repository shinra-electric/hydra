#include "hypervisor/hypervisor.hpp"

#include "common/common.hpp"
#include "horizon/horizon.hpp"
#include "hw/tegra_x1/cpu/hypervisor/cpu.hpp"
#include "hw/tegra_x1/mmu/hypervisor/mmu.hpp"
#include "hw/tegra_x1/mmu/memory.hpp"
#include <Hypervisor/hv_vcpu_types.h>

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
    cpu->SetReg(HV_REG_X1, 0x0000000F);

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
    // TODO: offset by thread id * some alignment?
    cpu->SetSysReg(HV_SYS_REG_TPIDRRO_EL0,
                   horizon.GetKernel().GetTlsMemory()->GetBase());

    // HACK: g_overrideHeapAddr is 0x5b982811ce0afbf4 for no reason
    //*((u64*)mmu->UnmapPtr(0x80017498)) = 0x0;
    // Logging::log(Logging::Level::Debug, "LoadROM HEAP OVERRIDE: 0x%08llx",
    //        *((u64*)mmu->UnmapPtr(0x80017498)));
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
                // Logging::log(Logging::Level::Debug, "VM made an HVC call! x0
                // register holds 0x%llx", x0);

                u64 esr = cpu->GetSysReg(HV_SYS_REG_ESR_EL1);
                u8 ec = (esr >> 26) & 0x3f;

                u64 elr = cpu->GetSysReg(HV_SYS_REG_ELR_EL1);

                u64 far = cpu->GetSysReg(HV_SYS_REG_FAR_EL1);

                // u64 spsr = cpu->GetSysReg(HV_SYS_REG_SPSR_EL1);
                // u64 mode = (spsr >> 2) & 0x3;

                u32 instruction = *((u32*)mmu->UnmapPtr(elr));

                switch (ec) {
                case 0x15:
                    // Debug
                    cpu->LogStackTrace(horizon.GetKernel().GetStackMemory(),
                                       elr);
                    // cpu->LogRegisters();

                    running =
                        horizon.GetKernel().SupervisorCall(cpu, esr & 0xffff);

                    // HACK
                    // cpu->SetSysReg(HV_SYS_REG_SPSR_EL1,
                    //               cpu->GetSysReg(HV_SYS_REG_SPSR_EL1) &
                    //               ~0x1);

                    break;
                case 0x18:
                    LOG_DEBUG(Hypervisor, "MSR MSR");
                    // TODO: implement
                    throw;
                    break;
                case 0x25:
                    // Debug
                    // cpu->LogStackTrace(horizon.GetKernel().GetStackMemory(),
                    //                   elr);

                    // LOG_DEBUG(Hypervisor,
                    //           "Data abort (PC: 0x{:08x}, FAR: 0x{:08x}, "
                    //           "instruction: 0x{:08x})",
                    //           elr, far, instruction);

                    // TODO: check if valid

                    DataAbort(instruction, far, elr);

                    break;
                default:
                    // Debug
                    cpu->LogStackTrace(horizon.GetKernel().GetStackMemory(),
                                       elr);

                    LOG_ERROR(
                        Hypervisor,
                        "Unknown HVC code (EC: 0x{:08x}, ESR: 0x{:08x}, PC: "
                        "0x{:08x}, FAR_ "
                        "0x{:08x})",
                        ec, esr, cpu->GetSysReg(HV_SYS_REG_ELR_EL1),
                        cpu->GetSysReg(HV_SYS_REG_FAR_EL1));
                    // Logging::log(Logging::Level::Debug, "X3: 0x%08llx",
                    // cpu->GetReg(HV_REG_X3));
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
                // Logging::log(Logging::Level::Debug, "VM made an SMC call! x0
                // register holds 0x%llx", x0);
                // Logging::log(Logging::Level::Debug, "Return to get on next
                // instruction.");
                LOG_WARNING(Hypervisor, "SMC instruction");

                cpu->AdvancePC();
            } else if (hvEc == 0x18) {
                // TODO: this should not happen

                // Debug
                cpu->LogStackTrace(horizon.GetKernel().GetStackMemory(),
                                   cpu->GetReg(HV_REG_PC));

                LOG_DEBUG(Hypervisor, "MSR MRS instruction");

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

                // std::cout << "Opcode: 0x" << std::hex << (int)opcode
                //           << std::endl;
                // std::cout << "First Operand (Rt): X" << std::dec << (int)rt
                //           << std::endl;
                // std::cout << "Second Operand (System Register): "
                //           << "op0=" << (int)op0 << ", op1=" << (int)op1
                //           << ", CRn=" << (int)crn << ", CRm=" << (int)crm
                //           << ", op2=" << (int)op2 << std::endl;

                cpu->SetReg((hv_reg_t)(HV_REG_X0 + rt), 0);

                // Set the return address
                // TODO: correct?
                // u64 elr = cpu->GetSysReg(HV_SYS_REG_ELR_EL1);
                // cpu->SetSysReg(HV_SYS_REG_ELR_EL1, elr + 4);
                cpu->AdvancePC();
            } else if (hvEc == 0x3C) { // BRK
                LOG_ERROR(Hypervisor, "BRK instruction");
                cpu->LogRegisters();
                // LOG_DEBUG(Hypervisor, "cbAlloc: 0x{:08x} -> 0x{:08x}",
                //           cpu->GetRegX(0) + 0x10,
                //           *((u64*)mmu->UnmapPtr(cpu->GetRegX(0) + 0x10)));
                break;
            } else {
                // Debug
                cpu->LogStackTrace(horizon.GetKernel().GetStackMemory(),
                                   cpu->GetReg(HV_REG_PC));
                cpu->LogRegisters();

                LOG_ERROR(
                    Hypervisor,
                    "Unexpected VM exception 0x{:08x} (EC: 0x{:08x}, ESR: "
                    "0x{:08x}, "
                    "VirtAddr: "
                    "0x{:08x}, IPA: 0x{:08x}, FAR: 0x{:08x})",
                    syndrome, hvEc, cpu->GetSysReg(HV_SYS_REG_ESR_EL1),
                    exit->exception.virtual_address,
                    exit->exception.physical_address,
                    cpu->GetSysReg(HV_SYS_REG_FAR_EL1));
                // Logging::log(Logging::Level::Debug, "X2: 0x%08llx",
                // cpu->GetReg(HV_REG_X2));
                //  Logging::log(Logging::Level::Debug, "INSTRUCTION: 0x%08x",
                //        *((u32*)horizon.GetKernel().UnmapPtr(
                //            cpu->GetReg(HV_REG_PC))));
                break;
            }
        } else if (exit->reason == HV_EXIT_REASON_VTIMER_ACTIVATED) {
            cpu->UpdateVTimer();
            LOG_DEBUG(Hypervisor, "VTimer");
        } else {
            // TODO: don't cast to u32
            LOG_ERROR(Hypervisor, "Unexpected VM exit reason {}",
                      (u32)exit->reason);
            break;
        }
    }
}

void Hypervisor::DataAbort(u32 instruction, u64 far, u64 elr) {
    // LOG_DEBUG(Hypervisor, "PC: 0x{:08x}, instruction: 0x{:08x}", elr,
    //           instruction);

    if ((instruction & 0xbfe00000) == 0x88400000) {           // LDAXR
        InterpretLDAXR(EXTRACT_BITS(instruction, 4, 0), far); // TODO: check
    } else if ((instruction & 0xbfe00000) == 0x88000000) {    // STLXR
        InterpretSTLXR(EXTRACT_BITS(instruction, 23, 16),
                       cpu->GetRegX(EXTRACT_BITS(instruction, 4, 0)),
                       far);                               // TODO: check
    } else if ((instruction & 0xff800000) == 0xd5000000) { // DC
        InterpretDC(far);
        // LOG_DEBUG(Hypervisor, "DC: 0x{:08x}",
        //           mmu->UnmapPtr(far));
        // cpu->LogRegisters(5);
    } else if ((instruction & 0xbfe00000) == 0xb8400000) { // LDR
        // TODO: doesn't seem work
        InterpretLDR(EXTRACT_BITS(instruction, 30, 30), 0,
                     EXTRACT_BITS(instruction, 4, 0), far);
    } else if ((instruction & 0xbe000000) ==
               0x3c000000) { // LDR and LDUR (simd) (TODO: correct?)
        InterpretLDR(1, 1, EXTRACT_BITS(instruction, 4, 0), far);
    } else if ((instruction & 0xbfc00000) == 0xb8000000) { // STR and STUR
        // TODO: support simd
        InterpretSTR(EXTRACT_BITS(instruction, 30, 30), 0,
                     EXTRACT_BITS(instruction, 4, 0), far);
    } else if ((instruction & 0x7b000000) == 0x29000000) { // LDP
        InterpretLDP(EXTRACT_BITS(instruction, 31, 31),
                     EXTRACT_BITS(instruction, 26, 26),
                     EXTRACT_BITS(instruction, 14, 10),
                     EXTRACT_BITS(instruction, 9, 5), far);
    } else if ((instruction & 0x7fc00000) == 0x28800000) { // STP
        // TODO: is this even necessary?
        InterpretSTP(EXTRACT_BITS(instruction, 31, 31),
                     EXTRACT_BITS(instruction, 26, 26),
                     EXTRACT_BITS(instruction, 14, 10),
                     EXTRACT_BITS(instruction, 9, 5), far);
    } else {
        cpu->LogStackTrace(horizon.GetKernel().GetStackMemory(), elr);
        LOG_WARNING(Hypervisor,
                    "Unimplemented data abort instruction "
                    "0x{:08x}",
                    instruction);
    }

    // Set the return address
    // TODO: correct?
    cpu->SetSysReg(HV_SYS_REG_ELR_EL1, elr + 4);
}

void Hypervisor::InterpretLDAXR(u8 out_reg, u64 addr) {
    // TODO: barrier

    u64 v = *((u64*)mmu->UnmapPtr(addr));

    cpu->SetRegX(out_reg, v);
    // LOG_DEBUG(Hypervisor, "loaded 0x{:08x} into X{} from 0x{:08x}", v,
    // out_reg,
    //           addr);
}

void Hypervisor::InterpretSTLXR(u8 out_res_reg, u64 v, u64 addr) {
    // TODO: barrier

    *((u64*)mmu->UnmapPtr(addr)) = v;

    cpu->SetRegX(out_res_reg, 0);
    // LOG_DEBUG(Hypervisor, "stored 0x{:08x} into 0x{:08x}, result reg X{}", v,
    //           addr, out_res_reg);
}

void Hypervisor::InterpretDC(u64 addr) {
    constexpr usize CACHE_LINE_SIZE = 0x40;

    // Zero out the memory
    memset((void*)mmu->UnmapPtr(addr), 0, CACHE_LINE_SIZE);
}

void Hypervisor::InterpretLDR(u8 size0, u8 size1, u8 out_reg, u64 addr) {
    u8 size = (4 << size0) << size1;

    switch (size) {
    case 4:
        cpu->SetRegX(out_reg, mmu->Load<u32>(addr));
        break;
    case 8:
        cpu->SetRegX(out_reg, mmu->Load<u64>(addr));
        break;
    case 16:
        cpu->SetRegQ(out_reg, mmu->Load<hv_simd_fp_uchar16_t>(addr));
        break;
    default:
        LOG_ERROR(Hypervisor, "Unsupported size: {}", size);
        break;
    }
}

void Hypervisor::InterpretSTR(u8 size0, u8 size1, u8 reg, u64 addr) {
    u8 size = (4 << size0) << size1;

    switch (size) {
    case 4:
        mmu->Store<u32>(addr, cpu->GetRegX(reg));
        break;
    case 8:
        mmu->Store<u64>(addr, cpu->GetRegX(reg));
        break;
    case 16:
        mmu->Store<hv_simd_fp_uchar16_t>(addr, cpu->GetRegQ(reg));
        break;
    default:
        LOG_ERROR(Hypervisor, "Unsupported size: {}", size);
        break;
    }
}

void Hypervisor::InterpretLDP(u8 size0, u8 size1, u8 out_reg0, u8 out_reg1,
                              u64 addr) {
    u8 size = (4 << size0) << size1;

    // LOG_DEBUG(Hypervisor, "size: {}, reg0: X{}, reg1: X{}, addr: 0x{:08x}",
    //           size * 8, out_reg0, out_reg1, addr);

    switch (size) {
    case 4:
        cpu->SetRegX(out_reg0, mmu->Load<u32>(addr));
        cpu->SetRegX(out_reg1, mmu->Load<u32>(addr + sizeof(u32)));
        break;
    case 8:
        cpu->SetRegX(out_reg0, mmu->Load<u64>(addr));
        cpu->SetRegX(out_reg1, mmu->Load<u64>(addr + sizeof(u64)));
        break;
    case 16:
        cpu->SetRegQ(out_reg0, mmu->Load<hv_simd_fp_uchar16_t>(addr));
        cpu->SetRegQ(out_reg1, mmu->Load<hv_simd_fp_uchar16_t>(
                                   addr + sizeof(hv_simd_fp_uchar16_t)));
        break;
    default:
        LOG_ERROR(Hypervisor, "Unsupported size: {}", size);
        break;
    }
}

void Hypervisor::InterpretSTP(u8 size0, u8 size1, u8 out_reg0, u8 out_reg1,
                              u64 addr) {
    u8 size = (4 << size0) << size1;

    // LOG_DEBUG(Hypervisor, "size: {}, reg0: X{}, reg1: X{}, addr: 0x{:08x}",
    //           size * 8, out_reg0, out_reg1, addr);

    switch (size) {
    case 4:
        mmu->Store<u32>(addr, cpu->GetRegX(out_reg0));
        mmu->Store<u32>(addr + sizeof(u32), cpu->GetRegX(out_reg1));
        break;
    case 8:
        mmu->Store<u64>(addr, cpu->GetRegX(out_reg0));
        mmu->Store<u64>(addr + sizeof(u64), cpu->GetRegX(out_reg1));
        break;
    case 16:
        mmu->Store<hv_simd_fp_uchar16_t>(addr, cpu->GetRegQ(out_reg0));
        mmu->Store<hv_simd_fp_uchar16_t>(addr + sizeof(hv_simd_fp_uchar16_t),
                                         cpu->GetRegQ(out_reg1));
        break;
    default:
        LOG_ERROR(Hypervisor, "Unsupported size: {}", size);
        break;
    }
}

} // namespace Hydra::Hypervisor
