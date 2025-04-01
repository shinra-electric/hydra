#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

TextureFormat to_texture_format(NvColorFormat color_format) {
#define NV_COLOR_FORMAT_CASE(color_format, texture_format)                     \
    case NvColorFormat::color_format:                                          \
        return TextureFormat::texture_format;

    // TODO: more formats
    switch (color_format) {
        NV_COLOR_FORMAT_CASE(A8B8G8R8, RGBA8Unorm)
        NV_COLOR_FORMAT_CASE(R5G6B5, B5G6R5Unorm)
    default:
        LOG_NOT_IMPLEMENTED(GPU, "NV color format {}", color_format);
        // TODO: don't throw
        throw;
        return TextureFormat::Invalid;
    }

#undef NV_COLOR_FORMAT_CASE
}

TextureFormat to_texture_format(const ImageFormatWord image_format_word) {
#define IMAGE_FORMAT_CASE(img_format, c_r, c_g, c_b, c_a, s_x, s_y, s_z, s_w,  \
                          texture_format)                                      \
    else if (image_format_word.image_format == ImageFormat::img_format &&      \
             image_format_word.component_r == ImageComponent::c_r &&           \
             image_format_word.component_g == ImageComponent::c_g &&           \
             image_format_word.component_b == ImageComponent::c_b &&           \
             image_format_word.component_a == ImageComponent::c_a &&           \
             image_format_word.swizzle_x == ImageSwizzle::s_x &&               \
             image_format_word.swizzle_y == ImageSwizzle::s_y &&               \
             image_format_word.swizzle_z == ImageSwizzle::s_z &&               \
             image_format_word.swizzle_w ==                                    \
                 ImageSwizzle::s_w) return TextureFormat::texture_format;

    // TODO: more formats
    if (image_format_word.image_format == ImageFormat::Invalid)
        return TextureFormat::Invalid;
    IMAGE_FORMAT_CASE(ARGB8, Unorm, Unorm, Unorm, Unorm, R, G, B, A,
                      RGBA8Unorm) // TODO: why argb?
    IMAGE_FORMAT_CASE(DXT1, Unorm, Unorm, Unorm, Unorm, R, G, B, OneFloat,
                      BC1_RGB)
    else {
        LOG_NOT_IMPLEMENTED(
            GPU,
            "Image format {}, components: {}, {}, {}, {}, swizzle: {}{}{}{}",
            image_format_word.image_format, image_format_word.component_r,
            image_format_word.component_g, image_format_word.component_b,
            image_format_word.component_a, image_format_word.swizzle_x,
            image_format_word.swizzle_y, image_format_word.swizzle_z,
            image_format_word.swizzle_w);
        // TODO: don't throw
        throw;
        return TextureFormat::Invalid;
    }

#undef IMAGE_FORMAT_CASE
}

TextureFormat to_texture_format(ColorSurfaceFormat color_surface_format) {
#define COLOR_SURFACE_FORMAT_CASE(color_surface_format, texture_format)        \
    case ColorSurfaceFormat::color_surface_format:                             \
        return TextureFormat::texture_format;

    // TODO: more formats
    switch (color_surface_format) {
        COLOR_SURFACE_FORMAT_CASE(Invalid, Invalid)
        COLOR_SURFACE_FORMAT_CASE(RGBA8Unorm, RGBA8Unorm)
    default:
        LOG_NOT_IMPLEMENTED(GPU, "Color surface format {}",
                            color_surface_format);
        // TODO: don't throw
        throw;
        return TextureFormat::Invalid;
    }

#undef COLOR_SURFACE_FORMAT_CASE
}

TextureFormat to_texture_format(DepthSurfaceFormat depth_surface_format) {
#define DEPTH_SURFACE_FORMAT_CASE(depth_surface_format, texture_format)        \
    case DepthSurfaceFormat::depth_surface_format:                             \
        return TextureFormat::texture_format;

    // TODO: more formats
    switch (depth_surface_format) {
        DEPTH_SURFACE_FORMAT_CASE(Invalid, Invalid)
    default:
        LOG_NOT_IMPLEMENTED(GPU, "Depth surface format {}",
                            depth_surface_format);
        // TODO: don't throw
        throw;
        return TextureFormat::Invalid;
    }

#undef DEPTH_SURFACE_FORMAT_CASE
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer
