#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

SwizzleChannels
get_texture_format_default_swizzle_channels(const TextureFormat format) {
#define SWIZZLE(r, g, b, a)                                                    \
    {ImageSwizzle::r, ImageSwizzle::g, ImageSwizzle::b, ImageSwizzle::a}

    // TODO: implement all formats
    switch (format) {
    case TextureFormat::RGBA8Unorm:
        return SWIZZLE(R, G, B, A);
    case TextureFormat::R8Unorm:
        return SWIZZLE(R, Zero, Zero, OneFloat);
    case TextureFormat::B5G6R5Unorm:
        return SWIZZLE(R, G, B, OneFloat);
    default:
        LOG_NOT_IMPLEMENTED(GPU, "{} default swizzle", format);
        return SWIZZLE(Zero, Zero, Zero, Zero);
    }

#undef SWIZZLE
}

TextureFormat to_texture_format(NvColorFormat color_format) {
#define NV_COLOR_FORMAT_CASE(color_format, texture_format)                     \
    case NvColorFormat::color_format:                                          \
        return TextureFormat::texture_format;

    // TODO: more formats
    switch (color_format) {
        NV_COLOR_FORMAT_CASE(A8B8G8R8, RGBA8Unorm)
        NV_COLOR_FORMAT_CASE(R5G6B5, B5G6R5Unorm)
        NV_COLOR_FORMAT_CASE(B5G6R5, B5G6R5Unorm)
    default:
        LOG_NOT_IMPLEMENTED(GPU, "NV color format {}", color_format);
        // TODO: don't throw
        throw;
        return TextureFormat::Invalid;
    }

#undef NV_COLOR_FORMAT_CASE
}

TextureFormat to_texture_format(const ImageFormatWord image_format_word) {
#define IMAGE_FORMAT_CASE(img_format, c_r, c_g, c_b, c_a, texture_format)      \
    else if (image_format_word.image_format == ImageFormat::img_format &&      \
             image_format_word.component_r == ImageComponent::c_r &&           \
             image_format_word.component_g == ImageComponent::c_g &&           \
             image_format_word.component_b == ImageComponent::c_b &&           \
             image_format_word.component_a ==                                  \
                 ImageComponent::c_a) return TextureFormat::texture_format;

    // TODO: more formats
    if (image_format_word.image_format == ImageFormat::Invalid)
        return TextureFormat::Invalid;
    IMAGE_FORMAT_CASE(ARGB8, Unorm, Unorm, Unorm, Unorm,
                      RGBA8Unorm) // TODO: why argb?
    IMAGE_FORMAT_CASE(DXT1, Unorm, Unorm, Unorm, Unorm, BC1_RGB)
    IMAGE_FORMAT_CASE(R8, Unorm, Unorm, Unorm, Unorm, R8Unorm)
    IMAGE_FORMAT_CASE(B5G6R5, Unorm, Unorm, Unorm, Unorm, B5G6R5Unorm)
    IMAGE_FORMAT_CASE(GR8, Unorm, Unorm, Unorm, Unorm,
                      RG8Unorm) // TODO: correct?
    else {
        LOG_NOT_IMPLEMENTED(
            GPU, "Image format {}, components: {}, {}, {}, {}",
            image_format_word.image_format, image_format_word.component_r,
            image_format_word.component_g, image_format_word.component_b,
            image_format_word.component_a);
        throw;
    }

#undef IMAGE_FORMAT_CASE
}

TextureFormat to_texture_format(ColorSurfaceFormat color_surface_format) {
#define COLOR_SURFACE_FORMAT_CASE(color_surface_format, texture_format)        \
    case ColorSurfaceFormat::color_surface_format:                             \
        return TextureFormat::texture_format;

    switch (color_surface_format) {
        COLOR_SURFACE_FORMAT_CASE(Invalid, Invalid)
        COLOR_SURFACE_FORMAT_CASE(Bitmap, Invalid)
        COLOR_SURFACE_FORMAT_CASE(Unknown1D, Invalid)
        COLOR_SURFACE_FORMAT_CASE(RGBA32Float, RGBA32Float)
        COLOR_SURFACE_FORMAT_CASE(RGBA32Sint, RGBA32Sint)
        COLOR_SURFACE_FORMAT_CASE(RGBA32Uint, RGBA32Uint)
        COLOR_SURFACE_FORMAT_CASE(RGBX32Float, RGBX32Float)
        COLOR_SURFACE_FORMAT_CASE(RGBX32Sint, RGBX32Sint)
        COLOR_SURFACE_FORMAT_CASE(RGBX32Uint, RGBX32Uint)
        COLOR_SURFACE_FORMAT_CASE(RGBA16Unorm, RGBA16Unorm)
        COLOR_SURFACE_FORMAT_CASE(RGBA16Snorm, RGBA16Snorm)
        COLOR_SURFACE_FORMAT_CASE(RGBA16Sint, RGBA16Sint)
        COLOR_SURFACE_FORMAT_CASE(RGBA16Uint, RGBA16Uint)
        COLOR_SURFACE_FORMAT_CASE(RGBA16Float, RGBA16Float)
        COLOR_SURFACE_FORMAT_CASE(RG32Float, RG32Float)
        COLOR_SURFACE_FORMAT_CASE(RG32Sint, RG32Sint)
        COLOR_SURFACE_FORMAT_CASE(RG32Uint, RG32Uint)
        COLOR_SURFACE_FORMAT_CASE(RGBX16Float, RGBX16Float)
        COLOR_SURFACE_FORMAT_CASE(BGRA8Unorm, BGRA8Unorm)
        COLOR_SURFACE_FORMAT_CASE(BGRA8Unorm_sRGB, BGRA8Unorm_sRGB)
        COLOR_SURFACE_FORMAT_CASE(RGB10A2Unorm, RGB10A2Unorm)
        COLOR_SURFACE_FORMAT_CASE(RGB10A2Uint, RGB10A2Uint)
        COLOR_SURFACE_FORMAT_CASE(RGBA8Unorm, RGBA8Unorm)
        COLOR_SURFACE_FORMAT_CASE(RGBA8Unorm_sRGB, RGBA8Unorm_sRGB)
        COLOR_SURFACE_FORMAT_CASE(RGBA8Snorm, RGBA8Snorm)
        COLOR_SURFACE_FORMAT_CASE(RGBA8Sint, RGBA8Sint)
        COLOR_SURFACE_FORMAT_CASE(RGBA8Uint, RGBA8Uint)
        COLOR_SURFACE_FORMAT_CASE(RG16Unorm, RG16Unorm)
        COLOR_SURFACE_FORMAT_CASE(RG16Snorm, RG16Snorm)
        COLOR_SURFACE_FORMAT_CASE(RG16Sint, RG16Sint)
        COLOR_SURFACE_FORMAT_CASE(RG16Uint, RG16Uint)
        COLOR_SURFACE_FORMAT_CASE(RG16Float, RG16Float)
        COLOR_SURFACE_FORMAT_CASE(BGR10A2Unorm, RGB10A2Unorm) // TODO: correct?
        COLOR_SURFACE_FORMAT_CASE(RG11B10Float, RG11B10Float)
        COLOR_SURFACE_FORMAT_CASE(R32Sint, R32Sint)
        COLOR_SURFACE_FORMAT_CASE(R32Uint, R32Uint)
        COLOR_SURFACE_FORMAT_CASE(R32Float, R32Float)
        COLOR_SURFACE_FORMAT_CASE(BGRX8Unorm, BGRX8Unorm)
        COLOR_SURFACE_FORMAT_CASE(BGRX8Unorm_sRGB, BGRX8Unorm_sRGB)
        COLOR_SURFACE_FORMAT_CASE(B5G6R5Unorm, B5G6R5Unorm)
        COLOR_SURFACE_FORMAT_CASE(BGR5A1Unorm, BGR5A1Unorm)
        COLOR_SURFACE_FORMAT_CASE(RG8Unorm, RG8Unorm)
        COLOR_SURFACE_FORMAT_CASE(RG8Snorm, RG8Snorm)
        COLOR_SURFACE_FORMAT_CASE(RG8Sint, RG8Sint)
        COLOR_SURFACE_FORMAT_CASE(RG8Uint, RG8Uint)
        COLOR_SURFACE_FORMAT_CASE(R16Unorm, R16Unorm)
        COLOR_SURFACE_FORMAT_CASE(R16Snorm, R16Snorm)
        COLOR_SURFACE_FORMAT_CASE(R16Sint, R16Sint)
        COLOR_SURFACE_FORMAT_CASE(R16Uint, R16Uint)
        COLOR_SURFACE_FORMAT_CASE(R16Float, R16Float)
        COLOR_SURFACE_FORMAT_CASE(R8Unorm, R8Unorm)
        COLOR_SURFACE_FORMAT_CASE(R8Snorm, R8Snorm)
        COLOR_SURFACE_FORMAT_CASE(R8Sint, R8Sint)
        COLOR_SURFACE_FORMAT_CASE(R8Uint, R8Uint)
        COLOR_SURFACE_FORMAT_CASE(A8Unorm, Invalid)     // TODO
        COLOR_SURFACE_FORMAT_CASE(BGR5X1Unorm, Invalid) // TODO
        COLOR_SURFACE_FORMAT_CASE(RGBX8Unorm, RGBX8Unorm)
        COLOR_SURFACE_FORMAT_CASE(RGBX8Unorm_sRGB, RGBX8Unorm_sRGB)
        COLOR_SURFACE_FORMAT_CASE(BGR5X1UnormUnknownFB, Invalid)
        COLOR_SURFACE_FORMAT_CASE(BGR5X1UnormUnknownFC, Invalid)
        COLOR_SURFACE_FORMAT_CASE(BGRX8UnormUnknownFD, Invalid)
        COLOR_SURFACE_FORMAT_CASE(BGRX8UnormUnknownFE, Invalid)
        COLOR_SURFACE_FORMAT_CASE(Y32UintUnknownFF, Invalid)
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

    switch (depth_surface_format) {
        DEPTH_SURFACE_FORMAT_CASE(Invalid, Invalid)
        DEPTH_SURFACE_FORMAT_CASE(Z32Float, Z32Float)
        DEPTH_SURFACE_FORMAT_CASE(Z16Unorm, Z16Unorm)
        DEPTH_SURFACE_FORMAT_CASE(S8Z24Unorm, Z24Unorm_S8Uint) // TODO
        DEPTH_SURFACE_FORMAT_CASE(Z24S8Unorm, Z24Unorm_S8Uint)
        DEPTH_SURFACE_FORMAT_CASE(S8Uint, S8Uint)
        DEPTH_SURFACE_FORMAT_CASE(Z24C8Unorm, Invalid)
        DEPTH_SURFACE_FORMAT_CASE(Z32S8X24Float, Invalid) // TODO
        DEPTH_SURFACE_FORMAT_CASE(Z24X8S8C8X16Unorm, Invalid)
        DEPTH_SURFACE_FORMAT_CASE(Z32X8C8X16Float, Invalid)
        DEPTH_SURFACE_FORMAT_CASE(Z32S8C8X16Float, Invalid)
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
