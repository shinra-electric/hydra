#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/integer_logical.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

// TODO: x, write_cc
void EmitLogical(DecoderContext& context, pred_t pred, bool pred_inv,
                 LogicOp op, PredicateOp pred_op, reg_t dst, pred_t dst_pred,
                 reg_t src_a, bool neg_a, ir::Value src_b, bool neg_b) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_a_v = neg_if(context.builder, ir::Value::Register(src_a), neg_a);
    auto src_b_v = neg_if(context.builder, src_b, neg_b);

    auto res = ir::Value::Undefined();
    switch (op) {
    case LogicOp::And:
        res = context.builder.OpBitwiseAnd(src_a_v, src_b_v);
        break;
    case LogicOp::Or:
        res = context.builder.OpBitwiseOr(src_a_v, src_b_v);
        break;
    case LogicOp::Xor:
        res = context.builder.OpBitwiseXor(src_a_v, src_b_v);
        break;
    case LogicOp::PassB:
        res = src_b_v;
        break;
    default:
        unreachable();
    }
    context.builder.OpCopy(ir::Value::Register(dst), res);

    auto pred_res = ir::Value::Undefined();
    switch (pred_op) {
    case PredicateOp::F:
        pred_res = ir::Value::Immediate(0); // TODO: false
        break;
    case PredicateOp::T:
        pred_res = ir::Value::Immediate(1); // TODO: true
        break;
    case PredicateOp::Z:
        pred_res = context.builder.OpCompare(ComparisonOp::Equal, res,
                                             ir::Value::Immediate(0));
        break;
    case PredicateOp::Nz:
        pred_res = context.builder.OpCompare(ComparisonOp::NotEqual, res,
                                             ir::Value::Immediate(0));
        break;
    default:
        unreachable();
    }
    context.builder.OpCopy(ir::Value::Predicate(dst_pred), pred_res);

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitLopR(DecoderContext& context, InstLopR inst) {
    EmitLogical(context, inst.base.pred, inst.base.pred_inv, inst.base.op,
                inst.base.pred_op, inst.base.dst, inst.base.dst_pred,
                inst.base.src_a, inst.base.neg_a,
                ir::Value::Register(inst.src_b), inst.base.neg_b);
}

void EmitLopC(DecoderContext& context, InstLopC inst) {
    EmitLogical(
        context, inst.base.pred, inst.base.pred_inv, inst.base.op,
        inst.base.pred_op, inst.base.dst, inst.base.dst_pred, inst.base.src_a,
        inst.base.neg_a,
        ir::Value::ConstMemory(CMem(inst.cbuf_slot, RZ, inst.cbuf_offset * 4)),
        inst.base.neg_b);
}

void EmitLopI(DecoderContext& context, InstLopI inst) {
    EmitLogical(context, inst.base.pred, inst.base.pred_inv, inst.base.op,
                inst.base.pred_op, inst.base.dst, inst.base.dst_pred,
                inst.base.src_a, inst.base.neg_a,
                ir::Value::Immediate((inst.imm_12 << 12) | (inst.imm_31 << 31)),
                inst.base.neg_b);
}

void EmitLop32I(DecoderContext& context, InstLop32I inst) {
    EmitLogical(context, inst.pred, inst.pred_inv, inst.op, PredicateOp::F,
                inst.dst, PT, inst.src_a, inst.neg_a,
                ir::Value::Immediate(inst.imm), false);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
