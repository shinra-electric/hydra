#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/iterator_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp {

class AllPathsIterator : public IteratorBase {
  public:
    using IteratorBase::IteratorBase;

    void Iterate(ObserverBase* observer) override;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp
