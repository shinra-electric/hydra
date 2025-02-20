#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class TextureBase;

class RendererBase {
  public:
    virtual ~RendererBase() {}

    // Texture
    virtual TextureBase* CreateTexture(const TextureDescriptor& descriptor) = 0;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
