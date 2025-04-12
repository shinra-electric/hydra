#include "core/hw/tegra_x1/gpu/engines/copy.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/buffer_base.hpp"
#include "core/hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(Copy, 0xc0, 1, LaunchDMA, LaunchDMAData)

void Copy::LaunchDMA(const u32 index, const LaunchDMAData data) {
    if (data.src_memory_layout == MemoryLayout::Pitch) {
        if (data.dst_memory_layout == MemoryLayout::Pitch) {
            LOG_NOT_IMPLEMENTED(Engines, "Copy buffer to buffer");
        } else {
            // TODO: don't multiply by 4
            memcpy((void*)UNMAP_ADDR(regs.offset_out),
                   (void*)UNMAP_ADDR(regs.offset_in),
                   regs.line_length_in * regs.line_count * 4);
            // TODO: do a GPU copy
            /*
            const auto src =
                GetBuffer(regs.offset_in,
                          regs.line_length_in *
                              regs.line_count); // TODO: is the size correct?
            const auto dst = GetBuffer(regs.offset_out,
                                       regs.line_length_in * regs.line_count);

            dst->CopyFrom(src);
            */
            // TODO: texture copy?
            /*
            auto texture =
                GetTexture(regs.offset_out_lo, regs.offset_out_hi, regs.dst);

            const auto& descriptor = texture->GetDescriptor();
            // TODO: use layer as origin Z in case of 3D textures
            texture->CopyFrom(
                buffer, regs.stride_in, regs.dst.layer,
                {regs.dst.origin.x, regs.dst.origin.y, 0},
                {descriptor.width, descriptor.height, regs.dst.depth});
                */
        }
    } else {
        if (data.dst_memory_layout == MemoryLayout::Pitch) {
            LOG_NOT_IMPLEMENTED(Engines, "Copy texture to buffer");
        } else {
            LOG_NOT_IMPLEMENTED(Engines, "Copy texture to texture");
        }
    }
}

Renderer::BufferBase* Copy::GetBuffer(const Iova addr, const usize size) {
    const Renderer::BufferDescriptor descriptor{
        .ptr = UNMAP_ADDR(addr),
        .size = size,
    };

    return RENDERER->GetBufferCache().Find(descriptor);
}

/*
Renderer::TextureBase* Copy::GetTexture(const u32 gpu_addr_lo,
                                        const u32 gpu_addr_hi,
                                        const TextureCopyInfo& info) {
    const auto gpu_addr = make_addr(gpu_addr_lo, gpu_addr_hi);

    i32 block_size_log2 = get_block_size_log2(info.block_size.height);
    LOG_DEBUG(Engines, "Block size: {}", 1 << block_size_log2);

    const Renderer::TextureDescriptor descriptor{
        .ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(gpu_addr),
        .format =
            Renderer::TextureFormat::RGBA8Unorm, // TODO: choose based on bpp
        .kind = NvKind::Pitch,                   // TODO: correct?
        .width = info.stride,                    // HACK
        .height = info.height,                   // HACK
        .block_height_log2 = 0,                  // TODO
        .stride = info.stride,
    };
    LOG_DEBUG(Engines, "COPYING: {}x{}", descriptor.width, descriptor.height);

    return RENDERER->GetTextureCache().GetTextureView(descriptor);
}
*/

} // namespace Hydra::HW::TegraX1::GPU::Engines
