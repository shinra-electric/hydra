#include "hw/tegra_x1/gpu/renderer/metal/texture.hpp"

#include "hw/tegra_x1/gpu/renderer/metal/buffer.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/maxwell_to_mtl.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

Texture::Texture(const TextureDescriptor& descriptor)
    : TextureBase(descriptor) {
    MTL::TextureDescriptor* desc = MTL::TextureDescriptor::alloc()->init();
    desc->setWidth(descriptor.width);
    desc->setHeight(descriptor.height);

    pixel_format = to_mtl_pixel_format(descriptor.format);
    desc->setPixelFormat(pixel_format);

    mtl_texture = Renderer::GetInstance().GetDevice()->newTexture(desc);
}

Texture::~Texture() { mtl_texture->release(); }

void Texture::CopyFrom(const void* data) {
    // TODO: do a GPU copy
    // TODO: bytes per image
    mtl_texture->replaceRegion(
        MTL::Region{0, 0, 0, mtl_texture->width(), mtl_texture->height(), 1}, 0,
        0, data, GetDescriptor().stride, 0);
}

void Texture::CopyFrom(const BufferBase* src, const usize src_stride,
                       const u32 dst_layer, const uint3 dst_origin,
                       const usize3 size) {
    const auto buffer_impl = static_cast<const Buffer*>(src);
    const auto mtl_buffer = buffer_impl->GetBuffer();

    auto encoder = Renderer::GetInstance().GetBlitCommandEncoder();

    // TODO: bytes per image
    encoder->copyFromBuffer(
        mtl_buffer, 0, src_stride, 0, MTL::Size(size.x(), size.y(), size.z()),
        mtl_texture, dst_layer, 0,
        MTL::Origin(dst_origin.x(), dst_origin.y(), dst_origin.z()));
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
