#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class TextureBase {
  public:
    TextureBase(const TextureDescriptor& descriptor_)
        : descriptor{descriptor_} {}
    virtual ~TextureBase() = default;

    // Getters
    const TextureDescriptor& GetDescriptor() const { return descriptor; }

  protected:
    const TextureDescriptor descriptor;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
