#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/instruction.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {

class Block {
  public:
    void AddInstructionWithDst(Opcode opcode, const std::optional<Value> dst,
                               const std::vector<Value>& operands = {}) {
        instructions.emplace_back(opcode, dst, operands);
    }

    Value AddInstruction(Opcode opcode,
                         const std::vector<Value>& operands = {}) {
        const auto dst = CreateLocal();
        AddInstructionWithDst(opcode, dst, operands);
        return dst;
    }

  private:
    std::vector<Instruction> instructions;

    inline Value CreateLocal() {
        return Value::Local(local_t(instructions.size()));
    }
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir
