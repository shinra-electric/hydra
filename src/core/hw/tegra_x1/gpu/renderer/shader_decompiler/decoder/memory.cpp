#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/memory.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

// TODO: todo
void EmitLoad(DecoderContext& context, pred_t pred, bool pred_inv,
              bool is_input, ALsSize size, reg_t dst, reg_t src,
              u32 src_offset) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    u32 count;
    switch (size) {
    case ALsSize::_32:
        count = 1;
        break;
    case ALsSize::_64:
        count = 2;
        break;
    case ALsSize::_96:
        count = 3;
        break;
    case ALsSize::_128:
        count = 4;
        break;
    default:
        count = 1;
        break;
    }

    for (u32 i = 0; i < count; i++) {
        context.builder.OpCopy(
            ir::Value::Register(dst + i),
            ir::Value::AttrMemory(
                AMem(src, src_offset + i * sizeof(u32), is_input)));
    }

    if (conditional)
        context.builder.OpEndIf();
}

// TODO: todo
void EmitStore(DecoderContext& context, pred_t pred, bool pred_inv,
               ALsSize size, reg_t dst, u32 dst_offset, reg_t src) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    u32 count;
    switch (size) {
    case ALsSize::_32:
        count = 1;
        break;
    case ALsSize::_64:
        count = 2;
        break;
    case ALsSize::_96:
        count = 3;
        break;
    case ALsSize::_128:
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

void EmitALd(DecoderContext& context, InstALd inst) {
    EmitLoad(context, inst.pred, inst.pred_inv, !inst.o, inst.size, inst.dst,
             inst.src, inst.src_offset);
}

void EmitASt(DecoderContext& context, InstASt inst) {
    EmitStore(context, inst.pred, inst.pred_inv, inst.size, inst.dst,
              inst.dst_offset, inst.src);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
