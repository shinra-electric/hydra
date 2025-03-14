#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class PipelineBase {
  public:
    PipelineBase(const PipelineDescriptor& descriptor_)
        : descriptor{descriptor_} {}
    virtual ~PipelineBase() = default;

    // Getters
    const PipelineDescriptor& GetDescriptor() const { return descriptor; }

  protected:
    const PipelineDescriptor descriptor;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
