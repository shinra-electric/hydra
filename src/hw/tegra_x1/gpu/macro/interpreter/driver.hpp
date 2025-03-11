#pragma once

#include "hw/tegra_x1/gpu/macro/driver_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter {

class Driver : public DriverBase {
  public:
  protected:
    void ExecuteImpl(u32 pc) override;

    void InstAlu(AluOperation op, u8 u32, u8 rB) override;
    void InstAddImmediate(u8 rA, u32 imm) override;
    void InstExtractInsert(u8 bA, u8 rA, u8 bB, u8 rB, u8 size) override;
    void InstExtractShiftLeftImmediate(u8 bA, u8 rA, u8 rB, u8 size) override;
    void InstExtractShiftLeftRegister(u8 rA, u8 bB, u8 rB, u8 size) override;
    void InstRead(u8 rA, u32 imm) override;
    void InstBranch(BranchCondition cond, u8 rA) override;
};

} // namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter
