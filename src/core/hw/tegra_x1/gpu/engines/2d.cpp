#include "core/hw/tegra_x1/gpu/engines/2d.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace hydra::hw::tegra_x1::gpu::engines {

DEFINE_METHOD_TABLE(TwoD, 0x237, 1, Copy, u32)

void TwoD::Copy(GMmu& gmmu, const u32 index,
                const u32 pixels_from_memory_src_y0_int) {
    auto& pixels = regs.pixels_from_memory;
    pixels.src_y0.integer = pixels_from_memory_src_y0_int;

    // TODO: can these also not be textures?
    auto src = GetTexture(gmmu, regs.src);
    auto dst = GetTexture(gmmu, regs.dst);

    f64 dudx = f64(pixels.dudx);
    f64 dvdy = f64(pixels.dvdy);
    // TODO: why are these params messed up?
    f64 src_x0 = f64(pixels.src_x0);
    f64 src_y0 = f64(pixels.src_y0);

    usize src_width = pixels.dst_width * dudx;
    usize src_height = pixels.dst_height * dvdy;

    dst->BlitFrom(src, regs.src.layer, {f32(src_x0), f32(src_y0), 0},
                  {src_width, src_height, 1}, regs.dst.layer,
                  {f32(pixels.dst_x0), f32(pixels.dst_y0), 0},
                  {pixels.dst_width, pixels.dst_height, 1});
}

renderer::TextureBase* TwoD::GetTexture(GMmu& gmmu, const Texture2DInfo& info) {
    const renderer::TextureDescriptor descriptor(
        gmmu.UnmapAddr(info.addr), renderer::to_texture_format(info.format),
        NvKind::Pitch, // TODO: correct?
        u32(info.width), u32(info.height),
        0, // HACK
           /*u32(info.stride)*/
        renderer::get_texture_format_stride(
            renderer::to_texture_format(info.format), info.width) // HACK
    );

    return RENDERER_INSTANCE.GetTextureCache().GetTextureView(descriptor);
}

} // namespace hydra::hw::tegra_x1::gpu::engines
