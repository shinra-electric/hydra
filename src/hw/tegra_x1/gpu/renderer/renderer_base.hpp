#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class TextureBase;

class RendererBase {
  public:
    virtual ~RendererBase() {}

    virtual void SetSurface(void* surface) = 0;

    virtual void Present(TextureBase* texture) = 0;

    // Texture
    virtual TextureBase* CreateTexture(const TextureDescriptor& descriptor) = 0;
    virtual void UploadTexture(TextureBase* texture, void* data,
                               usize pitch) = 0;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
