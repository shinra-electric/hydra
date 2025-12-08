#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

struct InstFaddR {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 src_b : 8;
    u64 _padding_28 : 11;
    u64 round_mode : 2; // TODO
    u64 _padding_41 : 3;
    u64 ftz : 1;
    u64 neg_b : 1;
    u64 abs_a : 1;
    u64 write_cc : 1;
    u64 neg_a : 1;
    u64 abs_b : 1;
    u64 sat : 1;
};

void EmitFaddR(DecoderContext& context, InstFaddR inst);

struct InstFaddC {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 cbuf_offset : 14;
    u64 cbuf_slot : 5;
    u64 round_mode : 2; // TODO
    u64 _padding_41 : 3;
    u64 ftz : 1;
    u64 neg_b : 1;
    u64 abs_a : 1;
    u64 write_cc : 1;
    u64 neg_a : 1;
    u64 abs_b : 1;
    u64 sat : 1;
};

void EmitFaddC(DecoderContext& context, InstFaddC inst);

struct InstFaddI {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 imm_12 : 19;
    u64 round_mode : 2; // TODO
    u64 _padding_41 : 3;
    u64 ftz : 1;
    u64 neg_b : 1;
    u64 abs_a : 1;
    u64 write_cc : 1;
    u64 neg_a : 1;
    u64 abs_b : 1;
    u64 sat : 1;
    u64 _padding_x51 : 5;
    u64 imm_31 : 1;
};

void EmitFaddI(DecoderContext& context, InstFaddI inst);

struct InstFadd32I {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 imm : 32;
    u64 write_cc : 1;
    u64 neg_b : 1;
    u64 abs_a : 1;
    u64 ftz : 1;
    u64 neg_a : 1;
    u64 abs_b : 1;
};

void EmitFadd32I(DecoderContext& context, InstFadd32I inst);

enum class MultiplyScale : u64 {
    None = 0,
    D2 = 1,
    D4 = 2,
    D8 = 3,
    M8 = 4,
    M4 = 5,
    M2 = 6,
};

struct InstFmulR {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 src_b : 8;
    u64 _padding_x28 : 11;
    u64 round_mode : 2; // TODO
    MultiplyScale scale : 3;
    u64 fmz : 2; // TODO
    u64 _padding_x46 : 1;
    u64 write_cc : 1;
    u64 neg_b : 1;
    u64 _padding_x49 : 1;
    u64 sat : 1;
};

void EmitFmulR(DecoderContext& context, InstFmulR inst);

struct InstFmulC {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 cbuf_offset : 14;
    u64 cbuf_slot : 5;
    u64 round_mode : 2; // TODO
    MultiplyScale scale : 3;
    u64 fmz : 2; // TODO
    u64 _padding_x46 : 1;
    u64 write_cc : 1;
    u64 neg_b : 1;
    u64 _padding_x49 : 1;
    u64 sat : 1;
};

void EmitFmulC(DecoderContext& context, InstFmulC inst);

struct InstFmulI {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 imm_12 : 19;
    u64 round_mode : 2; // TODO
    MultiplyScale scale : 3;
    u64 fmz : 2; // TODO
    u64 _padding_x46 : 1;
    u64 write_cc : 1;
    u64 neg_b : 1;
    u64 _padding_x49 : 1;
    u64 sat : 1;
    u64 _padding_x51 : 5;
    u64 imm_31 : 1;
};

void EmitFmulI(DecoderContext& context, InstFmulI inst);

struct InstFmul32I {
    u64 dst : 8;
    u64 src_a : 8;
    u64 pred : 3;
    u64 pred_inv : 1;
    u64 imm : 32;
    u64 write_cc : 1;
    u64 fmz : 2; // TODO
    u64 sat : 1;
};

void EmitFmul32I(DecoderContext& context, InstFmul32I inst);

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
