#pragma once

#include "hw/tegra_x1/gpu/const.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

struct PixelFormatInfo {
    MTL::PixelFormat pixel_format;
    bool has_stencil = false;
    // TODO: format decoder
};

const PixelFormatInfo& get_mtl_pixel_format_info(SurfaceFormat surface_format);

inline MTL::PixelFormat get_mtl_pixel_format(SurfaceFormat surface_format) {
    return get_mtl_pixel_format_info(surface_format).pixel_format;
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
