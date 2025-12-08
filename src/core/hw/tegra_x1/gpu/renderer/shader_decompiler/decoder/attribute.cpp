#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/attribute.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

void EmitStore(DecoderContext& context, pred_t pred, bool pred_inv,
               LsASize size, reg_t dst, u32 dst_offset, reg_t src) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    u32 count;
    switch (size) {
    case LsASize::_32:
        count = 1;
        break;
    case LsASize::_64:
        count = 2;
        break;
    case LsASize::_96:
        count = 3;
        break;
    case LsASize::_128:
        count = 4;
        break;
    default:
        count = 1;
        break;
    }

    for (u32 i = 0; i < count; i++) {
        context.builder.OpCopy(ir::Value::AttrMemory(AMem(
                                   dst, dst_offset + i * sizeof(u32), false)),
                               ir::Value::Register(src + i));
    }

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitStA(DecoderContext& context, InstStA inst) {
    EmitStore(context, inst.pred, inst.pred_inv, inst.size, inst.dst,
              inst.dst_offset, inst.src);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
