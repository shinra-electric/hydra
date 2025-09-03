#include "core/hw/tegra_x1/gpu/renderer/metal/texture.hpp"

#include "core/hw/tegra_x1/gpu/renderer/metal/buffer.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/maxwell_to_mtl.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::metal {

Texture::Texture(const TextureDescriptor& descriptor)
    : TextureBase(descriptor) {
    MTL::TextureDescriptor* desc = MTL::TextureDescriptor::alloc()->init();
    // TODO: type
    desc->setWidth(descriptor.width);
    desc->setHeight(descriptor.height);

    const auto& pixel_format_info = to_mtl_pixel_format_info(descriptor.format);
    pixel_format = pixel_format_info.pixel_format;
    desc->setPixelFormat(pixel_format);

    auto base_texture = METAL_RENDERER_INSTANCE.GetDevice()->newTexture(desc);
    if (pixel_format_info.component_indices == uchar4{0, 1, 2, 3}) {
        mtl_texture = base_texture;
    } else {
        // Swizzle
        static constexpr MTL::TextureSwizzle swizzle_components[] = {
            MTL::TextureSwizzleRed, MTL::TextureSwizzleGreen,
            MTL::TextureSwizzleBlue, MTL::TextureSwizzleAlpha};
        MTL::TextureSwizzleChannels swizzle_channels(
            swizzle_components[pixel_format_info.component_indices[0]],
            swizzle_components[pixel_format_info.component_indices[1]],
            swizzle_components[pixel_format_info.component_indices[2]],
            swizzle_components[pixel_format_info.component_indices[3]]);

        mtl_texture = base_texture->newTextureView(
            pixel_format_info.pixel_format, MTL::TextureType2D, NS::Range(0, 1),
            NS::Range(0, 1), swizzle_channels);
        base_texture->release();
    }
}

Texture::Texture(const TextureDescriptor& descriptor,
                 MTL::Texture* mtl_texture_)
    : TextureBase(descriptor), mtl_texture{mtl_texture_} {}

Texture::~Texture() { mtl_texture->release(); }

TextureBase* Texture::CreateView(const TextureViewDescriptor& descriptor) {
    const auto& pixel_format_info = to_mtl_pixel_format_info(descriptor.format);

    // Swizzle
    MTL::TextureSwizzle swizzle_components[] = {
        to_mtl_swizzle(descriptor.swizzle_channels.r),
        to_mtl_swizzle(descriptor.swizzle_channels.g),
        to_mtl_swizzle(descriptor.swizzle_channels.b),
        to_mtl_swizzle(descriptor.swizzle_channels.a)};
    MTL::TextureSwizzleChannels swizzle_channels(
        swizzle_components[pixel_format_info.component_indices[0]],
        swizzle_components[pixel_format_info.component_indices[1]],
        swizzle_components[pixel_format_info.component_indices[2]],
        swizzle_components[pixel_format_info.component_indices[3]]);

    // TODO: don't hardcode type, ranges and levels
    auto mtl_view = mtl_texture->newTextureView(
        to_mtl_pixel_format(descriptor.format), MTL::TextureType2D,
        NS::Range(0, 1), NS::Range(0, 1), swizzle_channels);

    auto desc = GetDescriptor();
    desc.format = descriptor.format;
    desc.swizzle_channels = descriptor.swizzle_channels;

    return new Texture(desc, mtl_view);
}

void Texture::CopyFrom(const uptr data) {
    mtl_texture->replaceRegion(
        MTL::Region(0, 0, 0, descriptor.width, descriptor.height, 1), 0,
        reinterpret_cast<void*>(data), descriptor.stride);
}

void Texture::CopyFrom(const BufferBase* src, const usize src_stride,
                       const u32 dst_layer, const uint3 dst_origin,
                       const usize3 size) {
    const auto mtl_src = static_cast<const Buffer*>(src)->GetBuffer();

    auto encoder = METAL_RENDERER_INSTANCE.GetBlitCommandEncoder();

    // TODO: bytes per image
    encoder->copyFromBuffer(
        mtl_src, 0, src_stride, 0, MTL::Size(size.x(), size.y(), size.z()),
        mtl_texture, dst_layer, 0,
        MTL::Origin(dst_origin.x(), dst_origin.y(), dst_origin.z()));
}

void Texture::CopyFrom(const TextureBase* src, const u32 src_layer,
                       const uint3 src_origin, const u32 dst_layer,
                       const uint3 dst_origin, const usize3 size) {
    const auto mtl_src = static_cast<const Texture*>(src)->GetTexture();

    auto encoder = METAL_RENDERER_INSTANCE.GetBlitCommandEncoder();

    // TODO: bytes per image
    encoder->copyFromTexture(
        mtl_src, src_layer, 0,
        MTL::Origin(src_origin.x(), src_origin.y(), src_origin.z()),
        MTL::Size(size.x(), size.y(), size.z()), mtl_texture, dst_layer, 0,
        MTL::Origin(dst_origin.x(), dst_origin.y(), dst_origin.z()));
}

void Texture::BlitFrom(const TextureBase* src, const u32 src_layer,
                       const float3 src_origin, const usize3 src_size,
                       const u32 dst_layer, const float3 dst_origin,
                       const usize3 dst_size) {
    // TODO: src layer
    METAL_RENDERER_INSTANCE.BlitTexture(
        static_cast<const Texture*>(src)->GetTexture(), src_origin, src_size,
        mtl_texture, dst_layer, dst_origin, dst_size);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::metal
