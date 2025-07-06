#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/block.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {

class Function {
  public:
    Block& GetBlock(label_t label) { return blocks[label]; }

  private:
    std::map<label_t, Block> blocks;

  public:
    CONST_REF_GETTER(blocks, GetBlocks);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir
