#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

union InstShlBase {
    BitField64<reg_t, 0, 8> dst;
    BitField64<reg_t, 8, 8> src_a;
    BitField64<pred_t, 16, 3> pred;
    BitField64<bool, 19, 1> pred_inv;
    BitField64<bool, 39, 1> m;
    BitField64<bool, 43, 1> x;
    BitField64<bool, 47, 1> write_cc;
};

union InstShlR {
    InstShlBase base;
    BitField64<reg_t, 20, 8> src_b;
};

void EmitShlR(DecoderContext& context, InstShlR inst);

union InstShlC {
    InstShlBase base;
    BitField64<u32, 20, 14> cbuf_offset;
    BitField64<u32, 34, 5> cbuf_slot;
};

void EmitShlC(DecoderContext& context, InstShlC inst);

union InstShlI {
    InstShlBase base;
    BitField64<u32, 20, 19> imm_12;
    BitField64<u32, 56, 1> imm_31;
};

void EmitShlI(DecoderContext& context, InstShlI inst);

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
