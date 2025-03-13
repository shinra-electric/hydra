#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class RenderPassBase {
  public:
    RenderPassBase(const RenderPassDescriptor& descriptor_)
        : descriptor{descriptor_} {}
    virtual ~RenderPassBase() = default;

  protected:
    const RenderPassDescriptor descriptor;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
