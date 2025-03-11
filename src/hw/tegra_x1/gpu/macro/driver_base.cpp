#include "hw/tegra_x1/gpu/macro/driver_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro {

void DriverBase::Execute(u32 index) {
    ExecuteImpl(macro_start_address_ram[index]);
}

void DriverBase::LoadInstructionRamPointer(u32 ptr) {
    macro_instruction_ram_ptr = ptr;
}

void DriverBase::LoadInstructionRam(u32 data) {
    macro_instruction_ram[macro_instruction_ram_ptr++] = data;
}

void DriverBase::LoadStartAddressRamPointer(u32 ptr) {
    macro_start_address_ram_ptr = ptr;
}

void DriverBase::LoadStartAddressRam(u32 data) {
    macro_start_address_ram[macro_start_address_ram_ptr++] = data;
}

void DriverBase::ParseInstruction(u32 pc) {
    u32 instruction = macro_instruction_ram[pc];
    LOG_DEBUG(GPU, "PC: 0x{:08x}, instruction: 0x{:08x}", pc, instruction);

#define GET_DATA(shift, mask) ((instruction >> shift) & mask)
#define GET_REG(shift) GET_DATA(shift, 0x7)
// TODO: rename
#define GET_B(shift) GET_DATA(shift, 0x1f)
#define GET_IMM() ((instruction >> 14) & 0x3ffff)
#define GET_SIZE(shift) GET_DATA(shift, 0x1f)

    Operation op = static_cast<Operation>(instruction & 0x3);
    switch (op) {
    case Operation::Alu: {
        auto alu_op = static_cast<AluOperation>(GET_DATA(17, 0x4));
        u8 rA = GET_REG(11);
        u8 rB = GET_REG(14);
        InstAlu(alu_op, rA, rB);
        break;
    }
    case Operation::AddImmediate: {
        u8 rA = GET_REG(11);
        u32 imm = GET_IMM();
        InstAddImmediate(rA, imm);
        break;
    }
    case Operation::ExtractInsert: {
        u8 bA = GET_B(27);
        u8 rA = GET_REG(11);
        u8 bB = GET_B(17);
        u8 rB = GET_REG(14);
        u8 size = GET_SIZE(22);
        InstExtractInsert(bA, rA, bB, rB, size);
        break;
    }
    case Operation::ExtractShiftLeftImmediate: {
        u8 bA = GET_B(27);
        u8 rA = GET_REG(11);
        u8 rB = GET_REG(14);
        u8 size = GET_SIZE(22);
        InstExtractShiftLeftImmediate(bA, rA, rB, size);
        break;
    }
    case Operation::ExtractShiftLeftRegister: {
        u8 rA = GET_REG(11);
        u8 bB = GET_B(17);
        u8 rB = GET_REG(14);
        u8 size = GET_SIZE(22);
        InstExtractShiftLeftRegister(rA, bB, rB, size);
        break;
    }
    case Operation::Read: {
        u8 rA = GET_REG(11);
        u32 imm = GET_IMM();
        InstRead(rA, imm);
        break;
    }
    case Operation::Branch: {
        auto cond = static_cast<BranchCondition>(GET_DATA(4, 0x3));
        u8 rA = GET_REG(11);
        InstBranch(cond, rA);
        break;
    }
    }
}

} // namespace Hydra::HW::TegraX1::GPU::Macro
