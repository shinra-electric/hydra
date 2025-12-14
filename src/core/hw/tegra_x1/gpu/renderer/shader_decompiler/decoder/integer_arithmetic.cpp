#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/integer_arithmetic.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

// TODO: extended, write_cc, sat
void EmitIadd(DecoderContext& context, pred_t pred, bool pred_inv,
              AvgMode avg_mode, reg_t dst, reg_t src_a, ir::Value src_b) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_a_v =
        NegIf(context.builder, ir::Value::Register(src_a, ir::ScalarType::I32),
              avg_mode == AvgMode::NegA);
    auto src_b_v = NegIf(context.builder, src_b, avg_mode == AvgMode::NegB);

    auto res = context.builder.OpAdd(src_a_v, src_b_v);
    if (avg_mode == AvgMode::PlusOne)
        res = context.builder.OpAdd(res, ir::Value::ConstantI(1));
    context.builder.OpCopy(ir::Value::Register(dst, ir::ScalarType::I32), res);

    if (conditional)
        context.builder.OpEndIf();
}

// TODO: write_cc
void EmitIscadd(DecoderContext& context, pred_t pred, bool pred_inv,
                AvgMode avg_mode, reg_t dst, reg_t src_a, u32 shift,
                ir::Value src_b) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_a_v =
        NegIf(context.builder, ir::Value::Register(src_a, ir::ScalarType::I32),
              avg_mode == AvgMode::NegA);
    src_a_v = context.builder.OpShiftLeft(src_a_v, ir::Value::ConstantU(shift));
    auto src_b_v = NegIf(context.builder, src_b, avg_mode == AvgMode::NegB);

    auto res = context.builder.OpAdd(src_a_v, src_b_v);
    if (avg_mode == AvgMode::PlusOne)
        res = context.builder.OpAdd(res, ir::Value::ConstantI(1));
    context.builder.OpCopy(ir::Value::Register(dst, ir::ScalarType::I32), res);

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitIaddR(DecoderContext& context, InstIaddR inst) {
    EmitIadd(context, inst.base.pred, inst.base.pred_inv, inst.base.avg_mode,
             inst.base.dst, inst.base.src_a,
             ir::Value::Register(inst.src_b, ir::ScalarType::I32));
}

void EmitIaddC(DecoderContext& context, InstIaddC inst) {
    EmitIadd(
        context, inst.base.pred, inst.base.pred_inv, inst.base.avg_mode,
        inst.base.dst, inst.base.src_a,
        ir::Value::ConstMemory(CMem(inst.cbuf_slot, RZ, inst.cbuf_offset * 4),
                               ir::ScalarType::I32));
}

void EmitIaddI(DecoderContext& context, InstIaddI inst) {
    EmitIadd(context, inst.base.pred, inst.base.pred_inv, inst.base.avg_mode,
             inst.base.dst, inst.base.src_a,
             ir::Value::Constant(GetIntImm20(inst.imm20_0, inst.imm20_19, true),
                                 ir::ScalarType::I32));
}

void EmitIadd32I(DecoderContext& context, InstIadd32I inst) {
    EmitIadd(context, inst.pred, inst.pred_inv, inst.avg_mode, inst.dst,
             inst.src_a, ir::Value::Constant(inst.imm, ir::ScalarType::I32));
}

void EmitIscaddR(DecoderContext& context, InstIscaddR inst) {
    EmitIscadd(context, inst.base.pred, inst.base.pred_inv, inst.base.avg_mode,
               inst.base.dst, inst.base.src_a, inst.base.shift,
               ir::Value::Register(inst.src_b, ir::ScalarType::I32));
}

void EmitIscaddC(DecoderContext& context, InstIscaddC inst) {
    EmitIscadd(
        context, inst.base.pred, inst.base.pred_inv, inst.base.avg_mode,
        inst.base.dst, inst.base.src_a, inst.base.shift,
        ir::Value::ConstMemory(CMem(inst.cbuf_slot, RZ, inst.cbuf_offset * 4),
                               ir::ScalarType::I32));
}

void EmitIscaddI(DecoderContext& context, InstIscaddI inst) {
    EmitIscadd(
        context, inst.base.pred, inst.base.pred_inv, inst.base.avg_mode,
        inst.base.dst, inst.base.src_a, inst.base.shift,
        ir::Value::Constant(GetIntImm20(inst.imm20_0, inst.imm20_19, true),
                            ir::ScalarType::I32));
}

void EmitIscadd32I(DecoderContext& context, InstIscadd32I inst) {
    EmitIscadd(context, inst.pred, inst.pred_inv, AvgMode::NoNeg, inst.dst,
               inst.src_a, inst.shift,
               ir::Value::Constant(inst.imm, ir::ScalarType::I32));
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
