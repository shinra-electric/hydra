#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

enum class ALsSize {
    _32 = 0,
    _64 = 1,
    _96 = 2,
    _128 = 3,
};

union InstALd {
    BitField64<reg_t, 0, 8> dst;
    BitField64<reg_t, 8, 8> src;
    BitField64<pred_t, 16, 3> pred;
    BitField64<bool, 19, 1> pred_inv;
    BitField64<u32, 20, 11> src_offset;
    BitField64<bool, 31, 1> p;
    BitField64<bool, 32, 1> o;
    BitField64<reg_t, 39, 8> todo; // TODO
    BitField64<ALsSize, 47, 2> size;
};

void EmitALd(DecoderContext& context, InstALd inst);

union InstASt {
    BitField64<reg_t, 0, 8> src;
    BitField64<reg_t, 8, 8> dst;
    BitField64<pred_t, 16, 3> pred;
    BitField64<bool, 19, 1> pred_inv;
    BitField64<u32, 20, 11> dst_offset;
    BitField64<bool, 31, 1> p;
    BitField64<reg_t, 39, 8> todo; // TODO
    BitField64<ALsSize, 47, 2> size;
};

void EmitASt(DecoderContext& context, InstASt inst);

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
