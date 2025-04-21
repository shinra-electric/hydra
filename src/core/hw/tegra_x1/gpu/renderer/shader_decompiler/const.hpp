#pragma once

#include "core/hw/tegra_x1/gpu/engines/const.hpp"
#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

#define INST0(value, mask) if ((inst & mask##ull) == value##ull)
#define INST(value, mask) else INST0(value, mask)

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

typedef u8 reg_t;

constexpr reg_t RZ = 255;

enum class DataType {
    None,
    Int,
    UInt,
    Float,
};

DataType to_data_type(Engines::VertexAttribType vertex_attrib_type);

inline DataType to_data_type(TextureFormat format) {
    // TODO: implement
    return DataType::Float;
}

struct AMem {
    reg_t reg;
    u64 imm;
};

struct CMem {
    u32 idx;
    reg_t reg;
    u64 imm;
};

enum class OperandType {
    Register,
    Immediate,
    AttributeMemory,
    ConstMemory,
};

struct Operand {
    OperandType type;
    union {
        reg_t reg;
        u32 imm;
        AMem amem;
        CMem cmem;
    };
    DataType data_type;

    static Operand Register(reg_t reg,
                            const DataType data_type = DataType::UInt) {
        return Operand{
            .type = OperandType::Register, .reg = reg, .data_type = data_type};
    }

    static Operand Immediate(u32 imm,
                             const DataType data_type = DataType::UInt) {
        return Operand{
            .type = OperandType::Immediate, .imm = imm, .data_type = data_type};
    }

    static Operand AttributeMemory(const AMem& amem,
                                   const DataType data_type = DataType::UInt) {
        return Operand{.type = OperandType::AttributeMemory,
                       .amem = amem,
                       .data_type = data_type};
    }

    static Operand ConstMemory(const CMem& cmem,
                               const DataType data_type = DataType::UInt) {
        return Operand{.type = OperandType::ConstMemory,
                       .cmem = cmem,
                       .data_type = data_type};
    }
};

enum class SVSemantic {
    Invalid,
    Position,
    UserInOut,
    // TODO: more
};

struct SV {
    SVSemantic semantic;
    u8 index;
    u8 component_index;
    // TODO: more?

    SV(SVSemantic semantic_, u8 index_ = invalid<u8>(),
       u8 component_index_ = invalid<u8>())
        : semantic{semantic_}, index{index_},
          component_index{component_index_} {}
};

enum class LoadStoreMode {
    Invalid,
    B32,
    B64,
    B96,
    B128,
};

u32 get_load_store_count(LoadStoreMode mode);

enum class MathFunc {
    Invalid,

    Cos,
    Sin,
    Ex2,
    Lg2,
    Rcp,
    Rsq,
    Rcp64h,
    Rsq64h,
    Sqrt,
};

const SV get_sv_from_addr(u64 addr);

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::OperandType, Register,
    "register", Immediate, "immediate", AttributeMemory, "attribute memory",
    ConstMemory, "const memory")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::DataType, Int, "int",
    UInt, "uint", Float, "float")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::SVSemantic, Invalid,
    "invalid", Position, "position", UserInOut, "user in out")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::LoadStoreMode, Invalid,
    "invalid", B32, "b32", B64, "b64", B96, "b96", B128, "b128")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::MathFunc, Cos, "cos",
    Sin, "sin", Ex2, "ex2", Lg2, "lg2", Rcp, "rcp", Rsq, "rsq", Rcp64h,
    "rcp64h", Rsq64h, "rsq64h", Sqrt, "sqrt")

#include "common/logging/log.hpp"
