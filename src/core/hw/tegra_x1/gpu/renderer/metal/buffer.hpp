#pragma once

#include "core/hw/tegra_x1/gpu/renderer/buffer_base.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::metal {

class Buffer final : public BufferBase {
  public:
    Buffer(u64 size);
    Buffer(MTL::Buffer* buffer_, u64 offset_);
    ~Buffer() override;

    uptr GetPtr() const override {
        return reinterpret_cast<uptr>(buffer->contents()) + offset;
    }

    // Copying
    void CopyFrom(const uptr data) override;
    void CopyFrom(TextureBase* src, const uint3 src_origin,
                  const uint3 src_size) override;

  private:
    MTL::Buffer* buffer;
    bool owns_buffer{true};
    u64 offset{0};

    void CopyFromImpl(BufferBase* src, u64 dst_offset, u64 src_offset,
                      u64 size) override;

  public:
    GETTER(buffer, GetBuffer);
    GETTER(offset, GetOffset);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::metal
