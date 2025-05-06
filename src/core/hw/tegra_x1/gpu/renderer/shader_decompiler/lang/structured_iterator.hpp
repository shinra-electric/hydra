#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/iterator_base.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/lang/structurizer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Lang {

class LangBuilderBase;

class StructuredIterator : public IteratorBase {
  public:
    StructuredIterator(Reader code_reader, const CfgBlock* root_block_)
        : IteratorBase(code_reader), root_block{root_block_} {}

    void Iterate(ObserverBase* observer) override;

  private:
    const CfgBlock* root_block;

    void IterateImpl(LangBuilderBase* builder, const CfgNode* node);
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Lang
