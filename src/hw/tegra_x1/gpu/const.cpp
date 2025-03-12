#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::HW::TegraX1::GPU {

// TODO: more formats
std::map<NvColorFormat, ColorSurfaceFormat> color_surface_format_lut = {
    {NvColorFormat::A8B8G8R8, ColorSurfaceFormat::RGBA8Unorm},
};

ColorSurfaceFormat to_color_surface_format(NvColorFormat color_format) {
    auto it = color_surface_format_lut.find(color_format);
    ASSERT_DEBUG(it != color_surface_format_lut.end(), GPU,
                 "Unknown NV color format {}", color_format);

    return it->second;
}

} // namespace Hydra::HW::TegraX1::GPU
