#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

struct InstIaddR {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 src_b : 8;
    u64 _padding_28 : 15;
    u64 x : 1;
    u64 _padding_44 : 3;
    u64 write_cc : 1;
    u64 neg_b : 1;
    u64 neg_a : 1;
    u64 sat : 1;
};

void EmitIaddR(DecoderContext& context, InstIaddR inst);

struct InstIaddC {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 cbuf_offset : 14;
    u64 cbuf_slot : 5;
    u64 _padding_39 : 4;
    u64 x : 1;
    u64 _padding_44 : 3;
    u64 write_cc : 1;
    u64 neg_b : 1;
    u64 neg_a : 1;
    u64 sat : 1;
};

void EmitIaddC(DecoderContext& context, InstIaddC inst);

struct InstIaddI {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 imm_12 : 19;
    u64 _padding_39 : 4;
    u64 x : 1;
    u64 _padding_44 : 3;
    u64 write_cc : 1;
    u64 neg_b : 1;
    u64 neg_a : 1;
    u64 sat : 1;
    u64 _padding_51 : 5;
    u64 imm_31 : 1;
};

void EmitIaddI(DecoderContext& context, InstIaddI inst);

struct InstIadd32I {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 imm : 32;
    u64 write_cc : 1;
    u64 x : 1;
    u64 sat : 1;
    u64 po_0 : 1;
    u64 neg_a : 1;
};

void EmitIadd32I(DecoderContext& context, InstIadd32I inst);

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
