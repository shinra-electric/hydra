#include "hw/tegra_x1/gpu/macro/interpreter/driver.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter {

void Driver::ExecuteImpl(u32 pc) {
    while (true) {
        if (ParseInstruction(pc++))
            break;
    }
}

u32 Driver::InstAlu(AluOperation op, u8 u32, u8 rB) {
    LOG_FUNC_NOT_IMPLEMENTED(Macro);
    LOG_DEBUG(Macro, "op: {}, u32: {}, rB: {}", op, u32, rB);

    return 0;
}

u32 Driver::InstAddImmediate(u8 rA, u32 imm) {
    LOG_DEBUG(Macro, "rA: {}, imm: 0x{:08x}", rA, imm);
    u32 value = GetReg(rA) + imm;

    return value;
}

u32 Driver::InstExtractInsert(u8 bA, u8 rA, u8 bB, u8 rB, u8 size) {
    LOG_FUNC_NOT_IMPLEMENTED(Macro);
    LOG_DEBUG(Macro, "bA: {}, rA: {}, bB: {}, rB: {}, size: {}", bA, rA, bB, rB,
              size);

    return 0;
}

u32 Driver::InstExtractShiftLeftImmediate(u8 bA, u8 rA, u8 rB, u8 size) {
    LOG_FUNC_NOT_IMPLEMENTED(Macro);
    LOG_DEBUG(Macro, "bA: {}, rA: {}, rB: {}, size: {}", bA, rA, rB, size);

    return 0;
}

u32 Driver::InstExtractShiftLeftRegister(u8 rA, u8 bB, u8 rB, u8 size) {
    LOG_FUNC_NOT_IMPLEMENTED(Macro);
    LOG_DEBUG(Macro, "rA: {}, bB: {}, rB: {}, size: {}", rA, bB, rB, size);

    return 0;
}

u32 Driver::InstRead(u8 rA, u32 imm) {
    LOG_FUNC_NOT_IMPLEMENTED(Macro);
    LOG_DEBUG(Macro, "rA: {}, imm: 0x{:08x}", rA, imm);

    return 0;
}

void Driver::InstBranch(BranchCondition cond, u8 rA) {
    LOG_FUNC_NOT_IMPLEMENTED(Macro);
    LOG_DEBUG(Macro, "cond: {}, rA: {}", cond, rA);
}

void Driver::InstResult(ResultOperation op, u8 rD, u32 value) {
    LOG_DEBUG(Macro, "result op: {}, rD: {}", op, rD);

    switch (op) {
    case ResultOperation::Move:
        SetReg(rD, value);
        break;
    default:
        LOG_NOT_IMPLEMENTED(Macro, "Result operation {}", op);
        break;
    }
}

} // namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter
