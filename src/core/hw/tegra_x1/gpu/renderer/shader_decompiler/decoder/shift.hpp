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
    BitField64<u32, 20, 19> imm20_0;
    BitField64<u32, 56, 1> imm20_19;
};

void EmitShlI(DecoderContext& context, InstShlI inst);

enum class XMode {
    Xlo = 1,
    Xmed = 2,
    Xhi = 3,
};

union InstShrBase {
    BitField64<reg_t, 0, 8> dst;
    BitField64<reg_t, 8, 8> src_a;
    BitField64<pred_t, 16, 3> pred;
    BitField64<bool, 19, 1> pred_inv;
    BitField64<bool, 39, 1> m;
    BitField64<bool, 40, 1> brev;
    BitField64<XMode, 43, 1> x_mode;
    BitField64<bool, 47, 1> write_cc;
    BitField64<bool, 48, 1> is_signed;
};

union InstShrR {
    InstShlBase base;
    BitField64<reg_t, 20, 8> src_b;
};

void EmitShrR(DecoderContext& context, InstShrR inst);

union InstShrC {
    InstShlBase base;
    BitField64<u32, 20, 14> cbuf_offset;
    BitField64<u32, 34, 5> cbuf_slot;
};

void EmitShrC(DecoderContext& context, InstShrC inst);

union InstShrI {
    InstShlBase base;
    BitField64<u32, 20, 19> imm20_0;
    BitField64<u32, 56, 1> imm20_19;
};

void EmitShrI(DecoderContext& context, InstShrI inst);

// TODO
/*
enum class XMode2 {
    Hi = 1,
    X = 2,
    Xhi = 3,
};

enum class MaxShift {
    U64 = 2,
    S64 = 3,
};

union InstShflrBase {
    BitField64<reg_t, 0, 8> dst;
    BitField64<reg_t, 8, 8> src_a;
    BitField64<pred_t, 16, 3> pred;
    BitField64<bool, 19, 1> pred_inv;
    BitField64<MaxShift, 37, 2> max_shift;
    BitField64<reg_t, 39, 8> src_c;
    BitField64<bool, 47, 1> write_cc;
    BitField64<XMode2, 48, 2> x_mode;
    BitField64<bool, 50, 1> m;
};

union InstShflrR {
    InstShflrBase base;
    BitField64<reg_t, 20, 8> src_b;
};

void EmitShflR(DecoderContext& context, InstShflrR inst);
void EmitShfrR(DecoderContext& context, InstShflrR inst);

union InstShflrI {
    InstShflrBase base;
    BitField64<u32, 20, 6> imm6;
};

void EmitShflI(DecoderContext& context, InstShflrI inst);
void EmitShfrI(DecoderContext& context, InstShflrI inst);
*/

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
