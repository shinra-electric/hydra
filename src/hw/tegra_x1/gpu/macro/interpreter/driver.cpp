#include "hw/tegra_x1/gpu/macro/interpreter/driver.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter {

void Driver::ExecuteImpl(u32 pc) {
    u32 i = 0;
    while (true) {
        ParseInstruction(pc++);

        // HACK
        if (i++ > 32)
            throw;
    }
}

void Driver::InstAlu(AluOperation op, u8 u32, u8 rB) {
    LOG_FUNC_NOT_IMPLEMENTED(GPU);
}

void Driver::InstAddImmediate(u8 rA, u32 imm) { LOG_FUNC_NOT_IMPLEMENTED(GPU); }

void Driver::InstExtractInsert(u8 bA, u8 rA, u8 bB, u8 rB, u8 size) {
    LOG_FUNC_NOT_IMPLEMENTED(GPU);
}

void Driver::InstExtractShiftLeftImmediate(u8 bA, u8 rA, u8 rB, u8 size) {
    LOG_FUNC_NOT_IMPLEMENTED(GPU);
}

void Driver::InstExtractShiftLeftRegister(u8 rA, u8 bB, u8 rB, u8 size) {
    LOG_FUNC_NOT_IMPLEMENTED(GPU);
}

void Driver::InstRead(u8 rA, u32 imm) { LOG_FUNC_NOT_IMPLEMENTED(GPU); }

void Driver::InstBranch(BranchCondition cond, u8 rA) {
    LOG_FUNC_NOT_IMPLEMENTED(GPU);
}

} // namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter
