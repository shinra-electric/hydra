#pragma once

#include "hw/tegra_x1/gpu/const.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

struct PixelFormatInfo {
    MTL::PixelFormat pixel_format;
    bool has_stencil = false;
    // TODO: format decoder
};

const PixelFormatInfo& to_mtl_pixel_format_info(SurfaceFormat surface_format);

inline MTL::PixelFormat to_mtl_pixel_format(SurfaceFormat surface_format) {
    return to_mtl_pixel_format_info(surface_format).pixel_format;
}

MTL::PrimitiveType
to_mtl_primitive_type(const Engines::PrimitiveType primitive_type);

const MTL::VertexFormat to_mtl_vertex_format(Engines::VertexAttribType type,
                                             Engines::VertexAttribSize size,
                                             bool bgra);

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
