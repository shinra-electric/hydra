#include "core/hw/tegra_x1/gpu/renderer/metal/buffer.hpp"

#include "core/hw/tegra_x1/gpu/renderer/metal/renderer.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/texture.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::metal {

Buffer::Buffer(const BufferDescriptor& descriptor) : BufferBase(descriptor) {
    buffer = METAL_RENDERER_INSTANCE.GetDevice()->newBuffer(
        reinterpret_cast<void*>(descriptor.ptr), descriptor.size,
        MTL::ResourceStorageModeShared, nullptr);
}

Buffer::Buffer(MTL::Buffer* buffer_, u32 offset_)
    : BufferBase({reinterpret_cast<uptr>(buffer_->contents()) + offset_,
                  buffer_->allocatedSize()}),
      buffer{buffer_}, offset{offset_} {
    owns_buffer = false;
}

Buffer::~Buffer() {
    if (owns_buffer)
        buffer->release();
}

void Buffer::CopyFrom(const uptr data) {
    memcpy(buffer->contents(), reinterpret_cast<void*>(data), descriptor.size);
}

void Buffer::CopyFrom(BufferBase* src) {
    auto src_impl = static_cast<Buffer*>(src);

    auto blit_encoder = METAL_RENDERER_INSTANCE.GetBlitCommandEncoder();
    blit_encoder->copyFromBuffer(src_impl->GetBuffer(), 0, buffer, 0,
                                 descriptor.size);
}

void Buffer::CopyFrom(TextureBase* src) {
    auto src_impl = static_cast<Texture*>(src);

    auto blit_encoder = METAL_RENDERER_INSTANCE.GetBlitCommandEncoder();
    // TODO: bytes per image
    blit_encoder->copyFromTexture(
        src_impl->GetTexture(), 0, 0, MTL::Origin::Make(0, 0, 0),
        MTL::Size::Make(src_impl->GetDescriptor().width,
                        src_impl->GetDescriptor().height, 1),
        buffer, 0, src_impl->GetDescriptor().stride, 0);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::metal
