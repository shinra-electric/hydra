#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/decoder/texture.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder {

namespace {

// TODO: nodep
void EmitTextureQuery(DecoderContext& context, pred_t pred, bool pred_inv,
                      TextureQuery query, reg_t dst, u8 write_mask, reg_t src,
                      u32 cbuf_index) {
    // TODO: src
    (void)src;

    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    switch (query) {
    case TextureQuery::Dimensions:
        for (u32 i = 0, mask = write_mask; mask != 0x0; i++, mask >>= 1) {
            if (mask & 1) {
                const auto res =
                    context.builder.OpTextureQueryDimension(cbuf_index, i);
                context.builder.OpCopy(
                    ir::Value::Register(dst + static_cast<u8>(i)), res);
            }
        }
        break;
    default:
        LOG_NOT_IMPLEMENTED(ShaderDecompiler, "Texture query {}", query);
        break;
    }

    if (conditional)
        context.builder.OpEndIf();
}

u8 ToTexComponent(TextureComponent component) {
    switch (component) {
    case TextureComponent::R:
        return 0;
    case TextureComponent::G:
        return 1;
    case TextureComponent::B:
        return 2;
    case TextureComponent::A:
        return 3;
    }
}

enum class ComponentSwizzle {
    None,
    Zero,
    X,
    Y,
    Z,
    W,
};

void CopyTextureResult(ir::Builder& builder, std::array<reg_t, 4> dsts,
                       ir::Value res,
                       std::array<ComponentSwizzle, 4> swizzles) {
    for (u32 i = 0; i < 4; i++) {
        const auto swizzle = swizzles[i];
        if (swizzle == ComponentSwizzle::None)
            continue;

        ir::Value value = ir::Value::Undefined();
        switch (swizzle) {
        case ComponentSwizzle::Zero:
            value = ir::Value::ConstantF(0.0f);
            break;
        case ComponentSwizzle::X:
            value = builder.OpVectorExtract(res, 0);
            break;
        case ComponentSwizzle::Y:
            value = builder.OpVectorExtract(res, 1);
            break;
        case ComponentSwizzle::Z:
            value = builder.OpVectorExtract(res, 2);
            break;
        case ComponentSwizzle::W:
            value = builder.OpVectorExtract(res, 3);
            break;
        default:
            unreachable();
        }

        builder.OpCopy(ir::Value::Register(dsts[i], ir::ScalarType::F32),
                       value);
    }
}

// TODO: nodep
void EmitTextureSample(DecoderContext& context, pred_t pred, bool pred_inv,
                       bool int_coords, TextureSampleTarget target, reg_t dst0,
                       reg_t dst1, u8 write_mask, reg_t src_a, reg_t src_b,
                       u32 cbuf_index) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

#define RA() ir::Value::Register(src_a++, ir::ScalarType::F32)
#define RB() ir::Value::Register(src_b++, ir::ScalarType::F32)

    TextureType type;
    TextureSampleFlags flags =
        (int_coords ? TextureSampleFlags::IntCoords : TextureSampleFlags::None);
    ir::Value array_index = ir::Value::Undefined();
    std::vector<ir::Value> coords;
    ir::Value cmp_value = ir::Value::Undefined();
    ir::Value lod = ir::Value::Undefined();
    switch (target) {
    case TextureSampleTarget::_1DLodZero:
        type = TextureType::_1D;
        flags = TextureSampleFlags::Lod;
        coords.push_back(RA());
        lod = ir::Value::ConstantF(0.0f);
        break;
    case TextureSampleTarget::_2D:
        type = TextureType::_2D;
        coords.push_back(RA());
        coords.push_back(RB());
        break;
    case TextureSampleTarget::_2DLodZero:
        type = TextureType::_2D;
        flags = TextureSampleFlags::Lod;
        coords.push_back(RA());
        coords.push_back(RB());
        lod = ir::Value::ConstantF(0.0f);
        break;
    case TextureSampleTarget::_2DLodLevel:
        type = TextureType::_2D;
        flags = TextureSampleFlags::Lod;
        coords.push_back(RA());
        coords.push_back(RA());
        lod = RB();
        break;
    case TextureSampleTarget::_2DDepthCompare:
        type = TextureType::_2D;
        flags = TextureSampleFlags::DepthCompare;
        coords.push_back(RA());
        coords.push_back(RA());
        cmp_value = RB();
        break;
    case TextureSampleTarget::_2DLodLevelDepthCompare:
        type = TextureType::_2D;
        flags = TextureSampleFlags::DepthCompare | TextureSampleFlags::Lod;
        coords.push_back(RA());
        coords.push_back(RA());
        cmp_value = RB();
        lod = RB();
        break;
    case TextureSampleTarget::_2DLodZeroDepthCompare:
        type = TextureType::_2D;
        flags = TextureSampleFlags::DepthCompare | TextureSampleFlags::Lod;
        coords.push_back(RA());
        coords.push_back(RA());
        cmp_value = RB();
        lod = ir::Value::ConstantF(0.0f);
        break;
    case TextureSampleTarget::_2DArray:
        type = TextureType::_2DArray;
        array_index = RA();
        coords.push_back(RA());
        coords.push_back(RB());
        break;
    case TextureSampleTarget::_2DArrayLodZero:
        type = TextureType::_2DArray;
        flags = TextureSampleFlags::Lod;
        array_index = RA();
        coords.push_back(RA());
        coords.push_back(RB());
        lod = ir::Value::ConstantF(0.0f);
        break;
    case TextureSampleTarget::_2DArrayLodZeroDepthCompare:
        type = TextureType::_2DArray;
        flags = TextureSampleFlags::DepthCompare | TextureSampleFlags::Lod;
        array_index = RA();
        coords.push_back(RA());
        coords.push_back(RB());
        cmp_value = RB();
        lod = ir::Value::ConstantF(0.0f);
        break;
    case TextureSampleTarget::_3D:
        type = TextureType::_3D;
        coords.push_back(RA());
        coords.push_back(RA());
        coords.push_back(RB());
        break;
    case TextureSampleTarget::_3DLodZero:
        type = TextureType::_3D;
        flags = TextureSampleFlags::Lod;
        coords.push_back(RA());
        coords.push_back(RA());
        coords.push_back(RB());
        lod = ir::Value::ConstantF(0.0f);
        break;
    case TextureSampleTarget::Cube:
        type = TextureType::Cube;
        coords.push_back(RA());
        coords.push_back(RA());
        coords.push_back(RB());
        break;
    case TextureSampleTarget::CubeLodLevel:
        type = TextureType::Cube;
        flags = TextureSampleFlags::Lod;
        coords.push_back(RA());
        coords.push_back(RA());
        coords.push_back(RB());
        lod = RB();
        break;
    }

    const auto coords_v =
        context.builder.OpVectorConstruct(ir::ScalarType::F32, coords);
    ir::Value res = context.builder.OpTextureSample(
        cbuf_index, type, flags, array_index, coords_v, cmp_value, lod);

    std::array<ComponentSwizzle, 4> swizzles;
    if (dst1 == RZ) {
        switch (write_mask) {
        case 0x0:
            swizzles = {ComponentSwizzle::X, ComponentSwizzle::None,
                        ComponentSwizzle::None, ComponentSwizzle::None};
            break;
        case 0x1:
            swizzles = {ComponentSwizzle::Y, ComponentSwizzle::None,
                        ComponentSwizzle::None, ComponentSwizzle::None};
            break;
        case 0x2:
            swizzles = {ComponentSwizzle::Z, ComponentSwizzle::None,
                        ComponentSwizzle::None, ComponentSwizzle::None};
            break;
        case 0x3:
            swizzles = {ComponentSwizzle::W, ComponentSwizzle::None,
                        ComponentSwizzle::None, ComponentSwizzle::None};
            break;
        case 0x4:
            swizzles = {ComponentSwizzle::X, ComponentSwizzle::Y,
                        ComponentSwizzle::None, ComponentSwizzle::None};
            break;
        case 0x5:
            swizzles = {ComponentSwizzle::X, ComponentSwizzle::W,
                        ComponentSwizzle::None, ComponentSwizzle::None};
            break;
        case 0x6:
            swizzles = {ComponentSwizzle::Y, ComponentSwizzle::W,
                        ComponentSwizzle::None, ComponentSwizzle::None};
            break;
        case 0x7:
            swizzles = {ComponentSwizzle::Z, ComponentSwizzle::W,
                        ComponentSwizzle::None, ComponentSwizzle::None};
            break;
        default:
            unreachable();
        }
    } else {
        switch (write_mask) {
        case 0x0:
            swizzles = {ComponentSwizzle::X, ComponentSwizzle::Y,
                        ComponentSwizzle::Z, ComponentSwizzle::None};
            break;
        case 0x1:
            swizzles = {ComponentSwizzle::X, ComponentSwizzle::Y,
                        ComponentSwizzle::W, ComponentSwizzle::None};
            break;
        case 0x2:
            swizzles = {ComponentSwizzle::X, ComponentSwizzle::Z,
                        ComponentSwizzle::W, ComponentSwizzle::None};
            break;
        case 0x3:
            swizzles = {ComponentSwizzle::Y, ComponentSwizzle::Z,
                        ComponentSwizzle::W, ComponentSwizzle::None};
            break;
        case 0x4:
            swizzles = {ComponentSwizzle::X, ComponentSwizzle::Y,
                        ComponentSwizzle::Z, ComponentSwizzle::W};
            break;
        default:
            LOG_WARN(ShaderDecompiler, "Invalid write mask {:#x}", write_mask);
            break;
        }
    }

    CopyTextureResult(context.builder, {dst0 + 0, dst0 + 1, dst1 + 0, dst1 + 1},
                      res, swizzles);

    if (conditional)
        context.builder.OpEndIf();
}

// TODO: ndv, nodep, dc, dst_pred, aoffi, lod, lc
void EmitTextureSample2(DecoderContext& context, pred_t pred, bool pred_inv,
                        bool int_coords, TextureDimension dim, reg_t dst,
                        u8 write_mask, reg_t src_a, reg_t src_b,
                        u32 cbuf_index) {
    (void)src_b;

    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

#define RA() ir::Value::Register(src_a++, ir::ScalarType::F32)
#define RB() ir::Value::Register(src_b++, ir::ScalarType::F32)

    TextureType type;
    TextureSampleFlags flags =
        (int_coords ? TextureSampleFlags::IntCoords : TextureSampleFlags::None);
    ir::Value array_index = ir::Value::Undefined();
    std::vector<ir::Value> coords;
    switch (dim) {
    case TextureDimension::_1D:
        type = TextureType::_1D;
        coords.push_back(RA());
        break;
    case TextureDimension::_1DArray:
        type = TextureType::_1DArray;
        array_index = RA();
        coords.push_back(RA());
        break;
    case TextureDimension::_2D:
        type = TextureType::_2D;
        coords.push_back(RA());
        coords.push_back(RA());
        break;
    case TextureDimension::_2DArray:
        type = TextureType::_2DArray;
        array_index = RA();
        coords.push_back(RA());
        coords.push_back(RA());
        break;
    case TextureDimension::_3D:
        type = TextureType::_3D;
        coords.push_back(RA());
        coords.push_back(RA());
        coords.push_back(RA());
        break;
    case TextureDimension::_3DArray:
        type = TextureType::_3DArray;
        array_index = RA();
        coords.push_back(RA());
        coords.push_back(RA());
        coords.push_back(RA());
        break;
    case TextureDimension::Cube:
        type = TextureType::Cube;
        coords.push_back(RA());
        coords.push_back(RA());
        coords.push_back(RA());
        break;
    case TextureDimension::CubeArray:
        type = TextureType::CubeArray;
        array_index = RA();
        coords.push_back(RA());
        coords.push_back(RA());
        coords.push_back(RA());
        break;
    }

    const auto coords_v =
        context.builder.OpVectorConstruct(ir::ScalarType::F32, coords);
    ir::Value res = context.builder.OpTextureSample(
        cbuf_index, type, flags, array_index, coords_v, ir::Value::Undefined(),
        ir::Value::Undefined());

    for (u8 i = 0; i < 4; i++) {
        if ((write_mask & (1 << i)) == 0x0)
            continue;

        context.builder.OpCopy(ir::Value::Register(dst++, ir::ScalarType::F32),
                               context.builder.OpVectorExtract(res, i));
    }

    if (conditional)
        context.builder.OpEndIf();
}

// TODO: dim, ndv, nodep, dc, offset, lc, dst_pred
void EmitTextureGather(DecoderContext& context, pred_t pred, bool pred_inv,
                       TextureComponent component, reg_t dst, u8 write_mask,
                       reg_t src_a, reg_t src_b, u32 cbuf_index) {
    // TODO: src B
    (void)src_b;

    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    const auto coords_v = context.builder.OpVectorConstruct(
        ir::ScalarType::F32,
        {ir::Value::Register(src_a + 0, ir::ScalarType::F32),
         ir::Value::Register(src_a + 1, ir::ScalarType::F32)});
    const auto res = context.builder.OpTextureGather(cbuf_index, coords_v,
                                                     ToTexComponent(component));
    CopyTextureResult(
        context.builder, {dst + 0, dst + 1, dst + 2, dst + 3}, res,
        {write_mask & 0x1 ? ComponentSwizzle::X : ComponentSwizzle::None,
         write_mask & 0x2 ? ComponentSwizzle::Y : ComponentSwizzle::None,
         write_mask & 0x4 ? ComponentSwizzle::Z : ComponentSwizzle::None,
         write_mask & 0x8 ? ComponentSwizzle::W : ComponentSwizzle::None});

    if (conditional)
        context.builder.OpEndIf();
}

// TODO: nodep, dc, aoffi
void EmitTextureGather2(DecoderContext& context, pred_t pred, bool pred_inv,
                        TextureComponent component, reg_t dst0, reg_t dst1,
                        reg_t src_a, reg_t src_b, u32 cbuf_index) {
    const auto conditional = HandlePredCond(context.builder, pred, pred_inv);

    const auto coords_v = context.builder.OpVectorConstruct(
        ir::ScalarType::F32, {ir::Value::Register(src_a, ir::ScalarType::F32),
                              ir::Value::Register(src_b, ir::ScalarType::F32)});
    const auto res = context.builder.OpTextureGather(cbuf_index, coords_v,
                                                     ToTexComponent(component));
    CopyTextureResult(context.builder, {dst0 + 0, dst0 + 1, dst1 + 0, dst1 + 1},
                      res,
                      {ComponentSwizzle::X, ComponentSwizzle::Y,
                       ComponentSwizzle::Z, ComponentSwizzle::W});

    if (conditional)
        context.builder.OpEndIf();
}

} // namespace

void EmitTxq(DecoderContext& context, InstTxq inst) {
    EmitTextureQuery(context, inst.pred, inst.pred_inv, inst.query, inst.dst,
                     inst.write_mask, inst.src, inst.cbuf_index);
}

void EmitTexs(DecoderContext& context, InstTexs inst) {
    EmitTextureSample(context, inst.pred, inst.pred_inv, false, inst.target,
                      inst.dst0, inst.dst1, inst.write_mask, inst.src_a,
                      inst.src_b, inst.cbuf_index);
}

void EmitTlds(DecoderContext& context, InstTlds inst) {
    EmitTextureSample(context, inst.pred, inst.pred_inv, true, inst.target,
                      inst.dst0, inst.dst1, inst.write_mask, inst.src_a,
                      inst.src_b, inst.cbuf_index);
}

void EmitTex(DecoderContext& context, InstTex inst) {
    EmitTextureSample2(context, inst.pred, inst.pred_inv, false, inst.dim,
                       inst.dst, inst.write_mask, inst.src_a, inst.src_b,
                       inst.cbuf_index);
}

void EmitTld4(DecoderContext& context, InstTld4 inst) {
    // TODO: why does this mess up SMO title screen?
    EmitTextureGather(context, inst.pred, inst.pred_inv, inst.component,
                      inst.dst, inst.write_mask, inst.src_a, inst.src_b,
                      inst.cbuf_index);
}

void EmitTld4s(DecoderContext& context, InstTld4s inst) {
    EmitTextureGather2(context, inst.pred, inst.pred_inv, inst.component,
                       inst.dst0, inst.dst1, inst.src_a, inst.src_b,
                       inst.cbuf_index);
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::decoder
