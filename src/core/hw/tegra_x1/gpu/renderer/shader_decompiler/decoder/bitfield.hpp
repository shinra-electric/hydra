#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

union InstBfeBase {
    BitField64<reg_t, 0, 8> dst;
    BitField64<reg_t, 8, 8> src_a;
    BitField64<pred_t, 16, 3> pred;
    BitField64<bool, 19, 1> pred_inv;
    BitField64<bool, 40, 1> brev;
    BitField64<bool, 47, 1> write_cc;
    BitField64<bool, 48, 1> is_signed;
};

union InstBfeR {
    InstBfeBase base;
    BitField64<reg_t, 20, 8> src_b;
};

void EmitBfeR(DecoderContext& context, InstBfeR inst);

union InstBfeC {
    InstBfeBase base;
    BitField64<u32, 20, 14> cbuf_offset;
    BitField64<u32, 34, 5> cbuf_slot;
};

void EmitBfeC(DecoderContext& context, InstBfeC inst);

union InstBfeI {
    InstBfeBase base;
    BitField64<u32, 20, 19> imm20_0;
    BitField64<u32, 56, 1> imm20_19;
};

void EmitBfeI(DecoderContext& context, InstBfeI inst);

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
