#include "core/hw/tegra_x1/gpu/renderer/metal/buffer.hpp"

#include "core/hw/tegra_x1/gpu/renderer/metal/renderer.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/texture.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::metal {

Buffer::Buffer(u64 size) : BufferBase(size) {
    buffer = METAL_RENDERER_INSTANCE.GetDevice()->newBuffer(
        size, MTL::ResourceStorageModeShared);
}

Buffer::Buffer(MTL::Buffer* buffer_, u64 offset_)
    : BufferBase(buffer_->allocatedSize() - offset_), buffer{buffer_},
      owns_buffer{false}, offset{offset_} {}

Buffer::~Buffer() {
    if (owns_buffer)
        buffer->release();
}

void Buffer::CopyFrom(const uptr data) {
    memcpy((u8*)buffer->contents() + offset, reinterpret_cast<void*>(data),
           size);
}

void Buffer::CopyFrom(TextureBase* src, const uint3 src_origin,
                      const uint3 src_size) {
    auto src_impl = static_cast<Texture*>(src);

    auto blit_encoder = METAL_RENDERER_INSTANCE.GetBlitCommandEncoder();
    // TODO: bytes per image
    // TODO: calculate the stride for the Metal pixel format
    blit_encoder->copyFromTexture(
        src_impl->GetTexture(), 0, 0,
        MTL::Origin::Make(src_origin.x(), src_origin.y(), src_origin.z()),
        MTL::Size::Make(src_size.x(), src_size.y(), src_size.z()), buffer,
        offset,
        get_texture_format_stride(src_impl->GetDescriptor().format,
                                  src_size.x()),
        0);
}

void Buffer::CopyFromImpl(BufferBase* src, u64 dst_offset, u64 src_offset,
                          u64 size) {
    auto src_impl = static_cast<Buffer*>(src);

    auto blit_encoder = METAL_RENDERER_INSTANCE.GetBlitCommandEncoder();
    blit_encoder->copyFromBuffer(src_impl->GetBuffer(),
                                 src_impl->GetOffset() + src_offset, buffer,
                                 offset + dst_offset, size);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::metal
