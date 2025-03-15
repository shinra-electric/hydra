#pragma once

#include "hw/tegra_x1/gpu/renderer/buffer_base.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

class Buffer final : public BufferBase {
  public:
    Buffer(const BufferDescriptor& descriptor);
    ~Buffer() override;

    // Getters
    MTL::Buffer* GetBuffer() const { return buffer; }

  private:
    MTL::Buffer* buffer;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
