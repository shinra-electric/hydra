#pragma once

#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "hw/tegra_x1/gpu/renderer/render_pass_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

class RenderPass final : public RenderPassBase {
  public:
    RenderPass(const RenderPassDescriptor& descriptor);
    ~RenderPass() override;

    // Getters
    MTL::RenderPassDescriptor* GetRenderPassDescriptor() const {
        return render_pass_descriptor;
    }

  private:
    MTL::RenderPassDescriptor* render_pass_descriptor;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
