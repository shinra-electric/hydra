#pragma once

#include "core/hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "core/hw/tegra_x1/gpu/renderer/pipeline_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

class Pipeline final : public PipelineBase {
  public:
    Pipeline(const PipelineDescriptor& descriptor);
    ~Pipeline() override;

    // Getters
    MTL::RenderPipelineState* GetPipeline() const { return pipeline; }

  private:
    MTL::RenderPipelineState* pipeline;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
