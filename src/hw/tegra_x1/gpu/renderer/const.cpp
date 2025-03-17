#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

TextureFormat to_texture_format(NvColorFormat color_format) {
#define NV_COLOR_FORMAT_CASE(color_format, texture_format)                     \
    case NvColorFormat::color_format:                                          \
        return TextureFormat::texture_format;

    // TODO: more formats
    switch (color_format) {
        NV_COLOR_FORMAT_CASE(A8B8G8R8, RGBA8Unorm)
    default:
        LOG_NOT_IMPLEMENTED(GPU, "NV color format {}", color_format);
        // TODO: don't throw
        throw;
        return TextureFormat::Invalid;
    }

#undef NV_COLOR_FORMAT_CASE
}

TextureFormat to_texture_format(ImageFormat image_format) {
#define IMAGE_FORMAT_CASE(image_format, texture_format)                        \
    case ImageFormat::image_format:                                            \
        return TextureFormat::texture_format;

    // TODO: more formats
    switch (image_format) {
        IMAGE_FORMAT_CASE(Invalid, Invalid)
        IMAGE_FORMAT_CASE(ARGB8, RGBA8Unorm) // TODO: why argb?
    default:
        LOG_NOT_IMPLEMENTED(GPU, "Image format {}", image_format);
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
