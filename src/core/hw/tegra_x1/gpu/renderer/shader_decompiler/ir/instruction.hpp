#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/value.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {

enum class Opcode {
    // Basic
    Copy,
    Neg,
    Not,
    Add,
    Multiply,
    Fma,
    ShiftLeft,
    ShiftRight,
    Cast,
    Compare,
    Bitwise,
    Select,

    // Control flow
    Branch,
    BranchConditional,
    BeginIf,
    EndIf,

    // Math
    Min,
    Max,
    MathFunction,

    // Vector
    VectorExtract,
    VectorInsert,
    VectorConstruct,

    // Special
    Exit,
    Discard,
    TextureSample,
    TextureRead,
};

constexpr usize MAX_INSTRUCTION_OPERANDS = 8;

struct Instruction {
  public:
    Instruction(Opcode opcode_, const std::optional<Value> dst_,
                const std::vector<Value>& operands_)
        : opcode{opcode_}, dst{dst_}, operands{operands_} {}

    bool HasDst() const { return dst.has_value(); }
    const Value& GetDst() const { return dst.value(); }

    usize GetOperandCount() const { return operands.size(); }
    const Value& GetOperand(usize index) const {
        ASSERT_DEBUG(index < GetOperandCount(), ShaderDecompiler,
                     "Invalid operand index (expected {}, got {})",
                     GetOperandCount(), index);
        return operands[index];
    }

    template <usize expected>
    inline void AssertOperandCount() const {
        ASSERT_DEBUG(GetOperandCount() == expected, ShaderDecompiler,
                     "Invalid operand count (expected {}, got {})", expected,
                     GetOperandCount());
    }

  private:
    Opcode opcode;
    std::optional<Value> dst;
    std::vector<Value> operands;

    public:
    GETTER(opcode, GetOpcode);
    CONST_REF_GETTER(operands, GetOperands);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir
