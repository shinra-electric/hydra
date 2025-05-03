#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/iterator/iterator_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Iterator {

class AllPathsIterator : public IteratorBase {
  public:
    using IteratorBase::IteratorBase;

    void Iterate(ObserverBase* observer) override;

  private:
    std::queue<u32> block_queue;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Iterator
