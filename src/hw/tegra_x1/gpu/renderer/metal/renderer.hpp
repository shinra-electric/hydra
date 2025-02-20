#pragma once

#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "hw/tegra_x1/gpu/renderer/renderer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

class Renderer : public RendererBase {
  public:
    Renderer();
    ~Renderer() override;

  private:
    MTL::Device* device;
    MTL::CommandQueue* command_queue;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
