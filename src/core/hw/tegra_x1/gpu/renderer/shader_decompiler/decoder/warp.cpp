#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/warp.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

void EmitShuffle(DecoderContext& context, pred_t pred, bool pred_inv,
                 ShuffleMode mode, reg_t dst, pred_t dst_pred, reg_t src_a,
                 ir::Value src_b, ir::Value src_c) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    const auto src_a_v = ir::Value::Register(src_a);

    // TODO: implement
    LOG_FUNC_WITH_ARGS_NOT_IMPLEMENTED(
        ShaderDecompiler, "mode: {}, src_a: {}, src_b: {}, src_c: {}", mode,
        src_a_v, src_b, src_c);
    // HACK
    const auto res = src_a_v;
    const auto pred_res = ir::Value::ConstantB(true);

    context.builder.OpCopy(ir::Value::Register(dst), res);
    context.builder.OpCopy(ir::Value::Predicate(dst_pred), pred_res);

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitShfl(DecoderContext& context, InstShfl inst) {
    EmitShuffle(context, inst.pred, inst.pred_inv, inst.mode, inst.dst,
                inst.dst_pred, inst.src_a,
                inst.b_is_imm ? ir::Value::ConstantU(inst.src_b_imm5)
                              : ir::Value::Register(inst.src_b_r),
                inst.c_is_imm ? ir::Value::ConstantU(inst.src_c_imm13)
                              : ir::Value::Register(inst.src_c_r));
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
