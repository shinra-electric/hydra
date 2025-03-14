#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU {

namespace Renderer {
class PipelineBase;
}

class PipelineCache : public CacheBase<PipelineCache, Renderer::PipelineBase*,
                                       Renderer::PipelineDescriptor> {
  public:
    void Destroy() {}

    Renderer::PipelineBase*
    Create(const Renderer::PipelineDescriptor& descriptor);
    void Update(Renderer::PipelineBase* pipeline) {}
    u64 Hash(const Renderer::PipelineDescriptor& descriptor);

    void DestroyElement(Renderer::PipelineBase* pipeline);
};

} // namespace Hydra::HW::TegraX1::GPU
