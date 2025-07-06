#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/block.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {

class Function {
  public:
    Block& GetBlock(label_t label) { return blocks[label]; }
    const Block& GetBlock(label_t label) const { return blocks.at(label); }

  private:
    std::map<label_t, Block> blocks;

  public:
    CONST_REF_GETTER(blocks, GetBlocks);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir

template <>
struct fmt::formatter<
    hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::Function>
    : formatter<string_view> {
    template <typename FormatContext>
    auto format(
        const hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::Function&
            func,
        FormatContext& ctx) const {
        std::string str = fmt::format("func {} {{\n", "TODO"); // TODO: name
        for (const auto& [label, block] : func.GetBlocks())
            str += fmt::format(
                "{}\n", block); // TODO: don't add newline for the last block
        str += "}\n";
        return formatter<string_view>::format(str, ctx);
    }
};
