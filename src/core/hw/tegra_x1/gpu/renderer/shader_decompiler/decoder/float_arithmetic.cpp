#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/float_arithmetic.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

// TODO: round_mode, ftz, write_cc, sat
void EmitFadd(DecoderContext& context, pred_t pred, bool pred_inv, reg_t dst,
              reg_t src_a, bool abs_a, bool neg_a, ir::Value src_b, bool abs_b,
              bool neg_b) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_a_v =
        abs_neg_if(context.builder, ir::Value::Register(src_a, DataType::F32),
                   abs_a, neg_a);
    auto src_b_v = abs_neg_if(context.builder, src_b, abs_b, neg_b);

    auto res = context.builder.OpAdd(src_a_v, src_b_v);
    context.builder.OpCopy(ir::Value::Register(dst, DataType::F32), res);

    if (conditional)
        context.builder.OpEndIf();
}

// TODO: round_mode, fmz, write_cc, sat
void EmitFmul(DecoderContext& context, pred_t pred, bool pred_inv,
              MultiplyScale scale, reg_t dst, reg_t src_a, ir::Value src_b,
              bool neg_b) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_a_v = ir::Value::Register(src_a, DataType::F32);
    if (scale != MultiplyScale::None) {
        f32 scale_f;
        switch (scale) {
        case MultiplyScale::M2:
            scale_f = 2.0f;
            break;
        case MultiplyScale::M4:
            scale_f = 4.0f;
            break;
        case MultiplyScale::M8:
            scale_f = 8.0f;
            break;
        case MultiplyScale::D2:
            scale_f = 0.5f;
            break;
        case MultiplyScale::D4:
            scale_f = 0.25f;
            break;
        case MultiplyScale::D8:
            scale_f = 0.125f;
            break;
        default:
            unreachable();
        }
        src_a_v = context.builder.OpMultiply(
            src_a_v,
            ir::Value::Immediate(std::bit_cast<u32>(scale_f), DataType::F32));
    }

    auto res = context.builder.OpMultiply(
        src_a_v, neg_if(context.builder, src_b, neg_b));
    context.builder.OpCopy(ir::Value::Register(dst, DataType::F32), res);

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitFaddR(DecoderContext& context, InstFaddR inst) {
    EmitFadd(context, inst.pred, inst.pred_inv, inst.dst, inst.src_a,
             inst.abs_a, inst.neg_a,
             ir::Value::Register(inst.src_b, DataType::F32), inst.abs_b,
             inst.neg_b);
}

void EmitFaddC(DecoderContext& context, InstFaddC inst) {
    EmitFadd(context, inst.pred, inst.pred_inv, inst.dst, inst.src_a,
             inst.abs_a, inst.neg_a,
             ir::Value::ConstMemory(
                 CMem(inst.cbuf_slot, RZ, inst.cbuf_offset * 4), DataType::F32),
             inst.abs_b, inst.neg_b);
}

void EmitFaddI(DecoderContext& context, InstFaddI inst) {
    EmitFadd(context, inst.pred, inst.pred_inv, inst.dst, inst.src_a,
             inst.abs_a, inst.neg_a,
             ir::Value::Immediate((inst.imm_12 << 12) | (inst.imm_31 << 31),
                                  DataType::F32),
             inst.abs_b, inst.neg_b);
}

void EmitFadd32I(DecoderContext& context, InstFadd32I inst) {
    EmitFadd(context, inst.pred, inst.pred_inv, inst.dst, inst.src_a,
             inst.abs_a, inst.neg_a,
             ir::Value::Immediate(inst.imm, DataType::F32), inst.abs_b,
             inst.neg_b);
}

void EmitFmulR(DecoderContext& context, InstFmulR inst) {
    EmitFmul(context, inst.pred, inst.pred_inv, inst.scale, inst.dst,
             inst.src_a, ir::Value::Register(inst.src_b, DataType::F32),
             inst.neg_b);
}

void EmitFmulC(DecoderContext& context, InstFmulC inst) {
    EmitFmul(context, inst.pred, inst.pred_inv, inst.scale, inst.dst,
             inst.src_a,
             ir::Value::ConstMemory(
                 CMem(inst.cbuf_slot, RZ, inst.cbuf_offset * 4), DataType::F32),
             inst.neg_b);
}

void EmitFmulI(DecoderContext& context, InstFmulI inst) {
    EmitFmul(context, inst.pred, inst.pred_inv, inst.scale, inst.dst,
             inst.src_a,
             ir::Value::Immediate((inst.imm_12 << 12) | (inst.imm_31 << 31),
                                  DataType::F32),
             inst.neg_b);
}

void EmitFmul32I(DecoderContext& context, InstFmul32I inst) {
    EmitFmul(context, inst.pred, inst.pred_inv, MultiplyScale::None, inst.dst,
             inst.src_a, ir::Value::Immediate(inst.imm, DataType::F32), false);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
