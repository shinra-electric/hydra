#pragma once

#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

class Texture : public TextureBase {
  public:
    Texture(const TextureDescriptor& descriptor);
    ~Texture() override;

  private:
    MTL::Texture* texture;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
