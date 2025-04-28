#include "core/hw/tegra_x1/gpu/renderer/texture_decoder.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

namespace {

static void decode_generic_16bx2(usize stride, usize height,
                                 usize block_height_log2, u8* in_data,
                                 u8* out_data) {
    const u32 block_height_gobs = 1U << block_height_log2;
    const u32 block_height_px = 8U << block_height_log2;

    const u32 width_blocks = stride >> 6;
    const u32 height_blocks =
        (height + block_height_px - 1) >> (3 + block_height_log2);

    // Clear the output buffer first
    // TODO: is this necessary?
    memset(out_data, 0, stride * height);

    for (u32 block_y = 0; block_y < height_blocks; block_y++) {
        for (u32 block_x = 0; block_x < width_blocks; block_x++) {
            for (u32 gob_y = 0; gob_y < block_height_gobs; gob_y++) {
                const u32 x = block_x * 64;
                const u32 y = block_y * block_height_px + gob_y * 8;
                if (y < height) {
                    u8* outgob = (u8*)out_data + y * stride + x;
                    // Reverse the 16Bx2 swizzling for each GOB
                    for (u32 i = 0; i < 32; i++) {
                        const u32 local_y = ((i >> 1) & 0x06) | (i & 0x01);
                        const u32 local_x =
                            ((i << 3) & 0x10) | ((i << 1) & 0x20);
                        *(u128*)(outgob + local_y * stride + local_x) =
                            *(u128*)in_data;
                        in_data += sizeof(u128);
                    }
                } else {
                    // Skip this GOB if we're past the valid height
                    in_data += 512;
                }
            }
        }
    }
}

} // namespace

TextureDecoder::TextureDecoder() {}
TextureDecoder::~TextureDecoder() {}

void TextureDecoder::Decode(const TextureDescriptor& descriptor, u8* out_data) {
    u8* in_data = reinterpret_cast<u8*>(descriptor.ptr);

    switch (descriptor.kind) {
    case NvKind::Pitch:
        // TODO: correct?
        out_data = in_data;
        break;
    case NvKind::Generic_16BX2:
        decode_generic_16bx2(descriptor.stride, descriptor.height,
                             descriptor.block_height_log2, in_data, out_data);
        break;
    default:
        LOG_ERROR(GPU, "Unimplemented texture kind {}", descriptor.kind);
        break;
    }
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer
