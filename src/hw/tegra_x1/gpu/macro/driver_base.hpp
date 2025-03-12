#pragma once

#include "hw/tegra_x1/gpu/macro/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro {

struct Result {
    ResultOperation op;
    u8 rD;
};

class DriverBase {
  public:
    void Execute(u32 index);

    void LoadInstructionRamPointer(u32 ptr);
    void LoadInstructionRam(u32 data);
    void LoadStartAddressRamPointer(u32 ptr);
    void LoadStartAddressRam(u32 data);

  protected:
    virtual void ExecuteImpl(u32 pc) = 0;

    virtual u32 InstAlu(AluOperation op, u8 rA, u8 rB) = 0;
    virtual u32 InstAddImmediate(u8 rA, u32 imm) = 0;
    virtual u32 InstExtractInsert(u8 bA, u8 rA, u8 bB, u8 rB, u8 size) = 0;
    virtual u32 InstExtractShiftLeftImmediate(u8 bA, u8 rA, u8 rB, u8 size) = 0;
    virtual u32 InstExtractShiftLeftRegister(u8 rA, u8 bB, u8 rB, u8 size) = 0;
    virtual u32 InstRead(u8 rA, u32 imm) = 0;
    virtual void InstBranch(BranchCondition cond, u8 rA) = 0;
    virtual void InstResult(ResultOperation op, u8 rD, u32 value) = 0;

    bool ParseInstruction(u32 pc);

  private:
    u32 macro_instruction_ram[0x1000] = {0}; // TODO: what should be the size?
    u32 macro_instruction_ram_ptr;
    u32 macro_start_address_ram[0x100] = {0}; // TODO: what should be the size?
    u32 macro_start_address_ram_ptr;

    u32 exit_after = 0;
};

} // namespace Hydra::HW::TegraX1::GPU::Macro
