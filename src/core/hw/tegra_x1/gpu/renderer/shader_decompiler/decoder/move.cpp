#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/move.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

// TODO: mask
void EmitMove(DecoderContext& context, pred_t pred, bool pred_inv, reg_t dst,
              ir::Value src) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    context.builder.OpCopy(ir::Value::Register(dst), src);

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitMovR(DecoderContext& context, InstMovR inst) {
    EmitMove(context, inst.base.pred, inst.base.pred_inv, inst.base.dst,
             ir::Value::Register(inst.src));
}

void EmitMovC(DecoderContext& context, InstMovC inst) {
    EmitMove(
        context, inst.base.pred, inst.base.pred_inv, inst.base.dst,
        ir::Value::ConstMemory(CMem(inst.cbuf_slot, RZ, inst.cbuf_offset * 4)));
}

void EmitMovI(DecoderContext& context, InstMovI inst) {
    EmitMove(
        context, inst.base.pred, inst.base.pred_inv, inst.base.dst,
        ir::Value::Immediate((inst.imm20_0 | (inst.imm20_19 << 19)) << 12));
}

void EmitMov32I(DecoderContext& context, InstMov32I inst) {
    EmitMove(context, inst.pred, inst.pred_inv, inst.dst,
             ir::Value::Immediate(inst.imm));
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
