#pragma once

#include "core/hw/tegra_x1/gpu/const.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

struct PixelFormatInfo {
    MTL::PixelFormat pixel_format;
    bool has_stencil = false;
    // TODO: format decoder
};

const PixelFormatInfo& to_mtl_pixel_format_info(TextureFormat format);

inline MTL::PixelFormat to_mtl_pixel_format(TextureFormat format) {
    return to_mtl_pixel_format_info(format).pixel_format;
}

MTL::PrimitiveType
to_mtl_primitive_type(const Engines::PrimitiveType primitive_type);

MTL::IndexType to_mtl_index_type(Engines::IndexType index_type);

const MTL::VertexFormat to_mtl_vertex_format(Engines::VertexAttribType type,
                                             Engines::VertexAttribSize size,
                                             bool bgra);

MTL::CompareFunction
to_mtl_compare_func(Engines::DepthTestFunc depth_test_func);

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
