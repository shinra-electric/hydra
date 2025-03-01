#include "hw/tegra_x1/gpu/renderer/metal/maxwell_to_mtl.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

#define PIXEL_FORMAT_ENTRY(color_format, pixel_format, has_stencil)            \
    {                                                                          \
        NvColorFormat::color_format, {                                         \
            MTL::PixelFormat##pixel_format, has_stencil                        \
        }                                                                      \
    }

#define COLOR_PIXEL_FORMAT_ENTRY(color_format, pixel_format)                   \
    PIXEL_FORMAT_ENTRY(color_format, pixel_format, false)

std::map<NvColorFormat, PixelFormatInfo> pixel_format_lut = {
    PIXEL_FORMAT_ENTRY(Unspecified, Invalid, false),
    PIXEL_FORMAT_ENTRY(NonColor8, Invalid, false),
    PIXEL_FORMAT_ENTRY(NonColor16, Invalid, false),
    PIXEL_FORMAT_ENTRY(NonColor24, Invalid, false),
    PIXEL_FORMAT_ENTRY(NonColor32, Invalid, false),
    COLOR_PIXEL_FORMAT_ENTRY(X4C4, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A4L4, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A8L8, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(Float_A16L16, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A1B5G5R5, A1BGR5Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(A4B4G4R4, ABGR4Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(A5B5G5R1, RGBA8Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(A2B10G10R10, RGB10A2Unorm), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(A8B8G8R8, RGBA8Unorm),      // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(A16B16G16R16, RGBA16Unorm), // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(Float_A16B16G16R16, RGBA16Float),
    COLOR_PIXEL_FORMAT_ENTRY(A1R5G5B5, A1BGR5Unorm),     // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(A4R4G4B4, ABGR4Unorm),      // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(A5R1G5B5, A1BGR5Unorm),     // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(A2R10G10B10, RGB10A2Unorm), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(A8R8G8B8, RGBA8Unorm),      // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(A1, A8Unorm),               // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(A2, A8Unorm),               // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(A4, A8Unorm),               // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(A8, A8Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(A16, RGBA16Unorm), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(A32, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(Float_A16, RGBA16Float), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(L4A4, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(L8A8, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(B4G4R4A4, ABGR4Unorm),  // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(B5G5R1A5, A1BGR5Unorm), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(B5G5R5A1, BGR5A1Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(B8G8R8A8, BGRA8Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(B10G10R10A2, BGR10A2Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(R1G5B5A5, A1BGR5Unorm), // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(R4G4B4A4, ABGR4Unorm),  // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(R5G5B5A1, BGR5A1Unorm), // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(R8G8B8A8, RGBA8Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(R10G10B10A2, RGB10A2Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(L1, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(L2, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(L4, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(L8, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(L16, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(L32, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(Float_L16, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(B5G6R5, B5G6R5Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(B6G5R5, RGBA8Unorm), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(B5G5R5X1, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(B8G8R8, BGRA8Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(B8G8R8X8, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(Float_B10G11R11,
                             RG11B10Float), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(X1B5G5R5, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(X8B8G8R8, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(X16B16G16R16, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(Float_X16B16G16R16, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(R3G3B2, B5G6R5Unorm), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(R5G5B6, B5G6R5Unorm), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(R5G6B5, B5G6R5Unorm), // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(R5G5B5X1, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(R8G8B8, RGBA8Unorm), // TODO: format decoder
    COLOR_PIXEL_FORMAT_ENTRY(R8G8B8X8, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(X1R5G5B5, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(X8R8G8B8, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(R8G8, RG8Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(R16G16, RG16Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(Float_R16G16, RG16Float),
    COLOR_PIXEL_FORMAT_ENTRY(R8, R8Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(R16, R16Unorm),
    COLOR_PIXEL_FORMAT_ENTRY(Float_R16, R16Float),
    COLOR_PIXEL_FORMAT_ENTRY(A2B10G10R10_sRGB, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A8B8G8R8_sRGB, RGBA8Unorm_sRGB), // TODO: swizzle
    COLOR_PIXEL_FORMAT_ENTRY(A16B16G16R16_sRGB, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A2R10G10B10_sRGB, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(B10G10R10A2_sRGB, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(R10G10B10A2_sRGB, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(X8B8G8R8_sRGB, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(X16B16G16R16_sRGB, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A2B10G10R10_709, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A8B8G8R8_709, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A16B16G16R16_709, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(A2R10G10B10_709, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(B10G10R10A2_709, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(R10G10B10A2_709, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(X8B8G8R8_709, Invalid),
    COLOR_PIXEL_FORMAT_ENTRY(X16B16G16R16_709, Invalid),
    // TODO: finish
};

const PixelFormatInfo& get_mtl_pixel_format_info(NvColorFormat color_format) {
    auto it = pixel_format_lut.find(color_format);
    ASSERT_DEBUG(it != pixel_format_lut.end(), MetalRenderer,
                 "Unknown color format {}", color_format);

    const auto& info = it->second;
    ASSERT_DEBUG(info.pixel_format != MTL::PixelFormatInvalid, MetalRenderer,
                 "Unimplemented color format {}", color_format);

    return info;
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
