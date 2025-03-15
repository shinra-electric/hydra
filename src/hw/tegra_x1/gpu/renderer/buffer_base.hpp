#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class BufferBase {
  public:
    BufferBase(const BufferDescriptor& descriptor_) : descriptor{descriptor_} {}
    virtual ~BufferBase() = default;

    // Getters
    const BufferDescriptor& GetDescriptor() const { return descriptor; }

  protected:
    const BufferDescriptor descriptor;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
