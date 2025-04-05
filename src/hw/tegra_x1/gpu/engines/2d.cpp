#include "hw/tegra_x1/gpu/engines/2d.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(TwoD, 0x237, 1, Copy, u32)

void TwoD::Copy(const u32 index, const u32 pixels_from_memory_src_y0_int) {
    // TODO: can these also not be textures?
    auto src = GetTexture(regs.src);
    auto dst = GetTexture(regs.dst);

    // TODO: dst origin
    dst->CopyFrom(
        src, regs.src.layer,
        {regs.pixels_from_memory.dst_x0, regs.pixels_from_memory.dst_y0, 0},
        regs.dst.layer, {0, 0, 0},
        {regs.pixels_from_memory.dst_width, regs.pixels_from_memory.dst_height,
         1});
}

Renderer::TextureBase* TwoD::GetTexture(const Texture2DInfo& info) {
    const Renderer::TextureDescriptor descriptor{
        .ptr = UNMAP_ADDR(info.addr),
        .format = Renderer::to_texture_format(info.format),
        .kind = NvKind::Pitch, // TODO: correct?
        .width = static_cast<usize>(info.width),
        .height = static_cast<usize>(info.height),
        .block_height_log2 = 0, // HACK
        .stride = static_cast<usize>(info.stride),
    };

    return RENDERER->GetTextureCache().Find(descriptor);
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
