#pragma once

#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {

class TextureBase;

class BufferBase {
  public:
    BufferBase(const BufferDescriptor& descriptor_) : descriptor{descriptor_} {}
    virtual ~BufferBase() = default;

    // Copying
    virtual void CopyFrom(const uptr data) = 0;
    virtual void CopyFrom(BufferBase* src) = 0;
    virtual void CopyFrom(TextureBase* src, const uint3 src_origin,
                          const uint3 src_size) = 0;

    // Getters
    const BufferDescriptor& GetDescriptor() const { return descriptor; }

  protected:
    const BufferDescriptor descriptor;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer
