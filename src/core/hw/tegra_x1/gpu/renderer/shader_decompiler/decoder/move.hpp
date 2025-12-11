#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

union InstMovBase {
    BitField64<reg_t, 0, 8> dst;
    BitField64<pred_t, 16, 3> pred;
    BitField64<bool, 19, 1> pred_inv;
    BitField64<u32, 39, 4> mask;
};

union InstMovR {
    InstMovBase base;
    BitField64<reg_t, 20, 8> src;
};

void EmitMovR(DecoderContext& context, InstMovR inst);

union InstMovC {
    InstMovBase base;
    BitField64<u32, 20, 14> cbuf_offset;
    BitField64<u32, 34, 5> cbuf_slot;
};

void EmitMovC(DecoderContext& context, InstMovC inst);

union InstMovI {
    InstMovBase base;
    BitField64<u32, 20, 19> imm20_0;
    BitField64<u32, 56, 1> imm20_19;
};

void EmitMovI(DecoderContext& context, InstMovI inst);

union InstMov32I {
    BitField64<reg_t, 0, 8> dst;
    BitField64<u32, 12, 4> mask;
    BitField64<pred_t, 16, 3> pred;
    BitField64<bool, 19, 1> pred_inv;
    BitField64<u32, 20, 32> imm;
};

void EmitMov32I(DecoderContext& context, InstMov32I inst);

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
