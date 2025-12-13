#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/conversion.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

DataType ToDataType(FloatFormat format) {
    switch (format) {
    case FloatFormat::F16:
        return DataType::F16;
    case FloatFormat::F32:
        return DataType::F32;
    case FloatFormat::F64:
        LOG_WARN(ShaderDecompiler, "F64 not supported");
        return DataType::F32;
    default:
        unreachable();
    }
}

// TODO: sh, ftz, write_cc
void EmitFloatToFloat(DecoderContext& context, pred_t pred, bool pred_inv,
                      RoundMode round_mode, bool saturate, reg_t dst,
                      FloatFormat dst_format, ir::Value src, bool abs,
                      bool neg) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_v = AbsNegIf(context.builder, src, abs, neg);
    switch (round_mode) {
    case RoundMode::Pass:
        break;
    case RoundMode::Round:
        src_v = context.builder.OpRound(src_v);
        break;
    case RoundMode::Floor:
        src_v = context.builder.OpFloor(src_v);
        break;
    case RoundMode::Ceil:
        src_v = context.builder.OpCeil(src_v);
        break;
    case RoundMode::Trunc:
        src_v = context.builder.OpTrunc(src_v);
        break;
    }

    const auto dst_type = ToDataType(dst_format);
    auto res = context.builder.OpCast(src_v, dst_type);
    res = SaturateIf(context.builder, res, saturate);
    context.builder.OpCopy(ir::Value::Register(dst, dst_type), res);

    if (conditional)
        context.builder.OpEndIf();
}

DataType ToDataType(IntegerFormat format) {
    switch (format) {
    case IntegerFormat::U16:
        return DataType::U16;
    case IntegerFormat::U32:
        return DataType::U32;
    case IntegerFormat::U64:
        LOG_WARN(ShaderDecompiler, "U64 not supported");
        return DataType::U32;
    case IntegerFormat::S16:
        return DataType::I16;
    case IntegerFormat::S32:
        return DataType::I32;
    case IntegerFormat::S64:
        LOG_WARN(ShaderDecompiler, "I64 not supported");
        return DataType::I32;
    default:
        unreachable();
    }
}

// TODO: sh, ftz, write_cc
void EmitFloatToInt(DecoderContext& context, pred_t pred, bool pred_inv,
                    RoundMode2 round_mode, reg_t dst, IntegerFormat dst_format,
                    ir::Value src, bool abs, bool neg) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    auto src_v = AbsNegIf(context.builder, src, abs, neg);
    switch (round_mode) {
    case RoundMode2::Round:
        src_v = context.builder.OpRound(src_v);
        break;
    case RoundMode2::Floor:
        src_v = context.builder.OpFloor(src_v);
        break;
    case RoundMode2::Ceil:
        src_v = context.builder.OpCeil(src_v);
        break;
    case RoundMode2::Trunc:
        src_v = context.builder.OpTrunc(src_v);
        break;
    }

    const auto dst_type = ToDataType(dst_format);
    const auto res = context.builder.OpCast(src_v, dst_type);
    context.builder.OpCopy(ir::Value::Register(dst, dst_type), res);

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitF2fR(DecoderContext& context, InstF2fR inst) {
    EmitFloatToFloat(
        context, inst.base.pred, inst.base.pred_inv, inst.base.GetRoundMode(),
        inst.base.sat, inst.base.dst, inst.base.dst_fmt,
        ir::Value::Register(inst.src, ToDataType(inst.base.src_fmt)),
        inst.base.abs, inst.base.neg);
}

void EmitF2fC(DecoderContext& context, InstF2fC inst) {
    EmitFloatToFloat(
        context, inst.base.pred, inst.base.pred_inv, inst.base.GetRoundMode(),
        inst.base.sat, inst.base.dst, inst.base.dst_fmt,
        ir::Value::ConstMemory(CMem(inst.cbuf_slot, RZ, inst.cbuf_offset * 4),
                               ToDataType(inst.base.src_fmt)),
        inst.base.abs, inst.base.neg);
}

void EmitF2fI(DecoderContext& context, InstF2fI inst) {
    EmitFloatToFloat(
        context, inst.base.pred, inst.base.pred_inv, inst.base.GetRoundMode(),
        inst.base.sat, inst.base.dst, inst.base.dst_fmt,
        ir::Value::Immediate((inst.imm20_0 | (inst.imm20_19 << 19)) << 12,
                             ToDataType(inst.base.src_fmt)),
        inst.base.abs, inst.base.neg);
}

void EmitF2iR(DecoderContext& context, InstF2iR inst) {
    EmitFloatToInt(context, inst.base.pred, inst.base.pred_inv,
                   inst.base.round_mode, inst.base.dst, inst.base.GetDstFmt(),
                   ir::Value::Register(inst.src, ToDataType(inst.base.src_fmt)),
                   inst.base.abs, inst.base.neg);
}

void EmitF2iC(DecoderContext& context, InstF2iC inst) {
    EmitFloatToInt(
        context, inst.base.pred, inst.base.pred_inv, inst.base.round_mode,
        inst.base.dst, inst.base.GetDstFmt(),
        ir::Value::ConstMemory(CMem(inst.cbuf_slot, RZ, inst.cbuf_offset * 4),
                               ToDataType(inst.base.src_fmt)),
        inst.base.abs, inst.base.neg);
}

void EmitF2iI(DecoderContext& context, InstF2iI inst) {
    EmitFloatToInt(
        context, inst.base.pred, inst.base.pred_inv, inst.base.round_mode,
        inst.base.dst, inst.base.GetDstFmt(),
        ir::Value::Immediate((inst.imm20_0 | (inst.imm20_19 << 19)) << 12,
                             ToDataType(inst.base.src_fmt)),
        inst.base.abs, inst.base.neg);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
