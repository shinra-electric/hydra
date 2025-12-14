#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/predicate_comparison.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

// TODO: write_cc
void EmitPredicateSet(DecoderContext& context, pred_t pred, bool pred_inv,
                      BoolOp b_op_ab, BoolOp b_op_c, bool b_float, reg_t dst,
                      pred_t src_a_pred, bool src_a_pred_inv, pred_t src_b_pred,
                      bool src_b_pred_inv, pred_t src_c_pred,
                      bool src_c_pred_inv) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_a_v = NegIf(context.builder, ir::Value::Predicate(src_a_pred),
                         src_a_pred_inv);
    auto src_b_v = NegIf(context.builder, ir::Value::Predicate(src_b_pred),
                         src_b_pred_inv);
    auto src_c_v = NegIf(context.builder, ir::Value::Predicate(src_c_pred),
                         src_c_pred_inv);

    auto res = GetLogical(context, b_op_ab, src_a_v, src_b_v);
    res = GetLogical(context, b_op_c, res, src_c_v);

    if (b_float) {
        res = context.builder.OpSelect(res, ir::Value::ConstantF(1.0f),
                                       ir::Value::ConstantF(0.0f));
        context.builder.OpCopy(ir::Value::Register(dst, ir::ScalarType::F32),
                               res);
    } else {
        context.builder.OpCopy(ir::Value::Register(dst), res);
    }

    if (conditional)
        context.builder.OpEndIf();
}

void EmitPredicateSetPredicate(DecoderContext& context, pred_t pred,
                               bool pred_inv, BoolOp b_op_ab, BoolOp b_op_c,
                               pred_t dst_pred, pred_t dst_inv_pred,
                               pred_t src_a_pred, bool src_a_pred_inv,
                               pred_t src_b_pred, bool src_b_pred_inv,
                               pred_t src_c_pred, bool src_c_pred_inv) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_a_v = NegIf(context.builder, ir::Value::Predicate(src_a_pred),
                         src_a_pred_inv);
    auto src_b_v = NegIf(context.builder, ir::Value::Predicate(src_b_pred),
                         src_b_pred_inv);
    auto src_c_v = NegIf(context.builder, ir::Value::Predicate(src_c_pred),
                         src_c_pred_inv);

    auto res0 = GetLogical(context, b_op_ab, src_a_v, src_b_v);
    auto res1 = context.builder.OpNot(res0);

    res0 = GetLogical(context, b_op_c, res0, src_c_v);
    res1 = GetLogical(context, b_op_c, res1, src_c_v);

    context.builder.OpCopy(ir::Value::Predicate(dst_pred), res0);
    context.builder.OpCopy(ir::Value::Predicate(dst_inv_pred), res1);

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitPset(DecoderContext& context, InstPset inst) {
    EmitPredicateSet(context, inst.pred, inst.pred_inv, inst.b_op_ab,
                     inst.b_op_c, inst.b_float, inst.dst, inst.src_a_pred,
                     inst.src_a_pred_inv, inst.src_b_pred, inst.src_b_pred_inv,
                     inst.src_c_pred, inst.src_c_pred_inv);
}

void EmitPsetp(DecoderContext& context, InstPsetp inst) {
    EmitPredicateSetPredicate(context, inst.pred, inst.pred_inv, inst.b_op_ab,
                              inst.b_op_c, inst.dst_pred, inst.dst_inv_pred,
                              inst.src_a_pred, inst.src_a_pred_inv,
                              inst.src_b_pred, inst.src_b_pred_inv,
                              inst.src_c_pred, inst.src_c_pred_inv);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
