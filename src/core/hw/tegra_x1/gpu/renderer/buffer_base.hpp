#pragma once

#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {

class TextureBase;

class BufferBase {
  public:
    BufferBase(u64 size_) : size{size_} {}
    virtual ~BufferBase() = default;

    virtual uptr GetPtr() const = 0;

    // Copying
    virtual void CopyFrom(const uptr data) = 0;
    void CopyFrom(BufferBase* src, u64 dst_offset = 0, u64 src_offset = 0,
                  u64 size = invalid<u64>()) {
        if (size == invalid<u64>())
            size = std::min(src->GetSize() - src_offset, size - dst_offset);

        CopyFromImpl(src, dst_offset, src_offset, size);
    }
    virtual void CopyFrom(TextureBase* src, const uint3 src_origin,
                          const uint3 src_size) = 0;

  protected:
    u64 size;

    virtual void CopyFromImpl(BufferBase* src, u64 dst_offset, u64 src_offset,
                              u64 size) = 0;

  public:
    GETTER(size, GetSize);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer
