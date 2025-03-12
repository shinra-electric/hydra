#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class TextureBase {
  public:
    TextureBase(const TextureDescriptor& descriptor_)
        : descriptor{descriptor_} {}
    virtual ~TextureBase() {}

    // Getters
    const TextureDescriptor& GetDescriptor() const { return descriptor; }

  private:
    TextureDescriptor descriptor;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
