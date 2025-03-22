#include "hw/tegra_x1/gpu/engines/copy.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/renderer/buffer_base.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(Copy, 0xc0, 1, LaunchDMA, LaunchDMAData)

void Copy::LaunchDMA(const u32 index, const LaunchDMAData data) {
    if (data.src_memory_layout == MemoryLayout::Pitch) {
        if (data.dst_memory_layout == MemoryLayout::Pitch) {
            LOG_NOT_IMPLEMENTED(Engines, "Copy buffer to buffer");
        } else {
            const auto buffer = GetBuffer(
                regs.offset_in_lo, regs.offset_in_hi,
                regs.stride_in * regs.dst.height); // TODO: is the size correct?
            auto texture =
                GetTexture(regs.offset_out_lo, regs.offset_out_hi, regs.dst);

            // TODO: proper width
            // TODO: use layer as origin Z in case of 3D textures
            texture->CopyFrom(
                buffer, regs.stride_in, regs.dst.layer,
                uint3({regs.dst.origin.x, regs.dst.origin.y, 0}),
                usize3({regs.dst.stride / 4, regs.dst.height, regs.dst.depth}));
        }
    } else {
        if (data.dst_memory_layout == MemoryLayout::Pitch) {
            LOG_NOT_IMPLEMENTED(Engines, "Copy texture to buffer");
        } else {
            LOG_NOT_IMPLEMENTED(Engines, "Copy texture to texture");
        }
    }
}

Renderer::BufferBase* Copy::GetBuffer(const u32 gpu_addr_lo,
                                      const u32 gpu_addr_hi, const usize size) {
    const auto gpu_addr = make_addr(gpu_addr_lo, gpu_addr_hi);

    const Renderer::BufferDescriptor descriptor{
        .ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(gpu_addr),
        .size = size,
    };

    return RENDERER->GetBufferCache().Find(descriptor);
}

Renderer::TextureBase* Copy::GetTexture(const u32 gpu_addr_lo,
                                        const u32 gpu_addr_hi,
                                        const TextureCopyInfo& info) {
    const auto gpu_addr = make_addr(gpu_addr_lo, gpu_addr_hi);

    const Renderer::TextureDescriptor descriptor{
        .ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(gpu_addr),
        .format =
            Renderer::TextureFormat::RGBA8Unorm, // TODO: choose based on bpp
        .kind = NvKind::Generic_16BX2,           // TODO: correct?
        .width = info.stride / 4,                // HACK
        .height = info.height,
        .block_height_log2 = 4, // HACK
        .stride = info.stride,
    };

    return RENDERER->GetTextureCache().Find(descriptor);
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
