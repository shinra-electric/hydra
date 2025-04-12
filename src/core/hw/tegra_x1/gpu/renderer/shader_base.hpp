#pragma once

#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class ShaderBase {
  public:
    ShaderBase(const ShaderDescriptor& descriptor_) : descriptor(descriptor_) {}
    virtual ~ShaderBase() = default;

    // Getters
    const ShaderDescriptor& GetDescriptor() const { return descriptor; }

  private:
    const ShaderDescriptor descriptor;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
