#include "core/hw/tegra_x1/gpu/engines/copy.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/memory_util.hpp"
#include "core/hw/tegra_x1/gpu/renderer/buffer_base.hpp"
#include "core/hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace hydra::hw::tegra_x1::gpu::engines {

DEFINE_METHOD_TABLE(Copy, 0xc0, 1, LaunchDMA, LaunchDMAData)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void Copy::LaunchDMA(GMmu& gmmu, const u32 index, const LaunchDMAData data) {
    // TODO: implement component remapping
    // HACK
    u32 src_stride = regs.src.stride;
    u32 dst_stride = regs.dst.stride;
    if (data.remap_enable) {
        const auto& c = regs.remap_components;
        ASSERT_DEBUG(
            c.dst_x == 0 && c.dst_y == 1 && c.dst_z == 2 && c.dst_w == 3,
            Engines, "Component remapping not implemented ({}, {}, {}, {})",
            c.dst_x, c.dst_y, c.dst_z, c.dst_w);

        const u32 component_size = c.component_size_minus_one + 1;
        {
            const u32 component_count = c.src_component_count_minus_one + 1;
            u32 bytes_per_block = component_count * component_size;
            src_stride *= bytes_per_block;
        }
        {
            const u32 component_count = c.dst_component_count_minus_one + 1;
            u32 bytes_per_block = component_count * component_size;
            dst_stride *= bytes_per_block;
        }
    }

    const auto src_ptr = gmmu.UnmapAddr(regs.offset_in);
    const auto dst_ptr = gmmu.UnmapAddr(regs.offset_out);
    if (data.src_memory_layout == MemoryLayout::Pitch) {
        if (data.dst_memory_layout == MemoryLayout::Pitch) {
            for (u32 i = 0; i < regs.line_count; i++)
                memcpy(reinterpret_cast<void*>(dst_ptr + regs.stride_out * i),
                       reinterpret_cast<void*>(src_ptr + regs.stride_in * i),
                       regs.stride_in);
        } else {
            // NOTE: a texture copy could be possible, as LineLengthIn contains
            // the width and PitchOut contains the stride, hence we could find
            // the pixel size and later use a texture view to alias the base
            // texture. However, this could break the order in which memory is
            // copied and create issues. Block formats could also be
            // problematic.

            // Encode as Generic 16BX2
            // TODO: block size log2 can also be negative?
            encode_generic_16bx2(dst_stride, regs.line_count,
                                 static_cast<u32>(get_block_size_log2(
                                     regs.dst.block_size.height)),
                                 reinterpret_cast<u8*>(src_ptr),
                                 reinterpret_cast<u8*>(dst_ptr));

            // memcpy((void*)gmmu.UnmapAddr(regs.offset_out),
            //        (void*)gmmu.UnmapAddr(regs.offset_in), stride *
            //        regs.line_count);

            /*
            const auto src =
                GetBuffer(regs.offset_in,
                          regs.line_length_in *
                              regs.line_count); // TODO: is the size correct?
            const auto dst = GetBuffer(regs.offset_out,
                                       regs.line_length_in * regs.line_count);

            dst->CopyFrom(src);
            */

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
            // TODO: block size log2 can also be negative?
            decode_generic_16bx2(src_stride, regs.line_count,
                                 static_cast<u32>(get_block_size_log2(
                                     regs.src.block_size.height)),
                                 reinterpret_cast<u8*>(src_ptr),
                                 reinterpret_cast<u8*>(dst_ptr));
        } else {
            LOG_NOT_IMPLEMENTED(Engines, "BlockLinear to BlockLinear");
        }
    }

    // TODO: correct?
    RENDERER_INSTANCE.GetTextureCache().NotifyGuestModifiedData(
        range<uptr>(dst_ptr, regs.stride_in * regs.line_count));
}

#pragma GCC diagnostic pop

/*
renderer::BufferBase* Copy::GetBuffer(GMmu& gmmu, const Iova addr, const usize
size) { const renderer::BufferDescriptor descriptor{ .ptr =
gmmu.UnmapAddr(addr), .size = size,
    };

    return RENDERER_INSTANCE.GetBufferCache().Find(descriptor);
}
*/

/*
renderer::TextureBase* Copy::GetTexture(const u32 gpu_addr_lo,
                                        const u32 gpu_addr_hi,
                                        const TextureCopyInfo& info) {
    const auto gpu_addr = make_addr(gpu_addr_lo, gpu_addr_hi);

    i32 block_size_log2 = get_block_size_log2(info.block_size.height);
    LOG_DEBUG(Engines, "Block size: {}", 1 << block_size_log2);

    const renderer::TextureDescriptor descriptor{
        .ptr = Gpu::GetInstance().GetGpuMmu().UnmapAddr(gpu_addr),
        .format =
            renderer::TextureFormat::RGBA8Unorm, // TODO: choose based on bpp
        .kind = NvKind::Pitch,                   // TODO: correct?
        .width = info.stride,                    // HACK
        .height = info.height,                   // HACK
        .block_height_log2 = 0,                  // TODO
        .stride = info.stride,
    };
    LOG_DEBUG(Engines, "COPYING: {}x{}", descriptor.width, descriptor.height);

    return RENDERER_INSTANCE.GetTextureCache().GetTextureView(descriptor);
}
*/

} // namespace hydra::hw::tegra_x1::gpu::engines
