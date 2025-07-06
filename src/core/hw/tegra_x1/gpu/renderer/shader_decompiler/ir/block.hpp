#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/instruction.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {

class Block {
  public:
    Block(const label_t label_) : label{label_} {}

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
    const label_t label;
    std::vector<Instruction> instructions;

    inline Value CreateLocal() {
        return Value::Local(local_t(instructions.size()));
    }

  public:
    GETTER(label, GetLabel);
    CONST_REF_GETTER(instructions, GetInstructions);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir

template <>
struct fmt::formatter<
    hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::Block>
    : formatter<string_view> {
    template <typename FormatContext>
    auto
    format(const hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::Block&
               block,
           FormatContext& ctx) const {
        std::string str = fmt::format("{}:\n", block.GetLabel());
        for (const auto& inst : block.GetInstructions())
            str += fmt::format("{}\n", inst);
        return formatter<string_view>::format(str, ctx);
    }
};
