#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::HW::TegraX1::GPU {

// TODO: more formats
static std::map<NvColorFormat, SurfaceFormat> surface_format_lut = {
    {NvColorFormat::A8B8G8R8, SurfaceFormat::RGBA8Unorm},
};

SurfaceFormat to_surface_format(NvColorFormat color_format) {
    auto it = surface_format_lut.find(color_format);
    ASSERT_DEBUG(it != surface_format_lut.end(), GPU,
                 "Unknown NV color format {}", color_format);

    return it->second;
}

} // namespace Hydra::HW::TegraX1::GPU
