#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/cfg.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/iterator_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp {

class CfgIterator : public IteratorBase {
  public:
    CfgIterator(const DecompilerContext& context, Reader code_reader,
                const std::map<u32, CfgBasicBlock*>& blocks_)
        : IteratorBase(context, code_reader), blocks{blocks_} {}

    void Iterate(ObserverBase* observer) override;

  private:
    const std::map<u32, CfgBasicBlock*>& blocks;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp
