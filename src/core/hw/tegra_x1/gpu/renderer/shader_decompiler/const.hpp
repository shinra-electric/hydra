#pragma once

#include "core/hw/tegra_x1/gpu/engines/const.hpp"
#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

#define INST0(value, mask) if ((inst & mask##ull) == value##ull)
#define INST(value, mask) else INST0(value, mask)

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

typedef u8 reg_t;
typedef u8 pred_t;

constexpr reg_t RZ = 255;
constexpr pred_t PT = 7;

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
    Predicate,
    Immediate,
    AttributeMemory,
    ConstMemory,
};

struct Operand {
    OperandType type;
    union {
        reg_t reg;
        pred_t pred;
        u32 imm;
        AMem amem;
        CMem cmem;
    };
    DataType data_type;
    bool neg;

    static Operand Register(reg_t reg,
                            const DataType data_type = DataType::UInt,
                            bool neg = false) {
        return Operand{.type = OperandType::Register,
                       .reg = reg,
                       .data_type = data_type,
                       .neg = neg};
    }

    static Operand Predicate(pred_t pred, bool neg = false) {
        return Operand{.type = OperandType::Predicate,
                       .pred = pred,
                       .data_type = DataType::None,
                       .neg = neg};
    }

    static Operand Immediate(u32 imm, const DataType data_type = DataType::UInt,
                             bool neg = false) {
        return Operand{.type = OperandType::Immediate,
                       .imm = imm,
                       .data_type = data_type,
                       .neg = neg};
    }

    static Operand AttributeMemory(const AMem& amem,
                                   const DataType data_type = DataType::UInt,
                                   bool neg = false) {
        return Operand{.type = OperandType::AttributeMemory,
                       .amem = amem,
                       .data_type = data_type,
                       .neg = neg};
    }

    static Operand ConstMemory(const CMem& cmem,
                               const DataType data_type = DataType::UInt,
                               bool neg = false) {
        return Operand{.type = OperandType::ConstMemory,
                       .cmem = cmem,
                       .data_type = data_type,
                       .neg = neg};
    }
};

struct PredCond {
    pred_t pred;
    bool not_;
    bool never;
};

enum class SvSemantic {
    Invalid,
    Position,
    UserInOut,
    // TODO: more
};

struct Sv {
    SvSemantic semantic;
    u8 index;
    // TODO: more?

    Sv(SvSemantic semantic_, u8 index_ = invalid<u8>())
        : semantic{semantic_}, index{index_} {}

    bool const operator==(const Sv& o) const {
        return semantic == o.semantic && index == o.index;
    }

    bool const operator<(const Sv& o) const {
        return semantic < o.semantic ||
               (semantic == o.semantic && index < o.index);
    }
};

struct SvAccess {
    Sv sv;
    u8 component_index;

    SvAccess(const Sv& sv_, u8 component_index_)
        : sv{sv_}, component_index{component_index_} {}
};

const SvAccess get_sv_access_from_addr(u64 addr);

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

enum class ComparisonOperator {
    Invalid,

    F, // TODO: what is this?
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Num, // TODO: what is this?
    Nan, // TODO: what is this?
    LessU,
    EqualU,
    LessEqualU,
    GreaterU,
    NotEqualU,
    GreaterEqualU,
    T, // TODO: what is this?
};

enum class BinaryOperator {
    Invalid,

    And,
    Or,
    Xor,
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::OperandType, Register,
    "register", Immediate, "immediate", AttributeMemory, "attribute memory",
    ConstMemory, "const memory")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::DataType, Int, "int",
    UInt, "uint", Float, "float")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::SvSemantic, Invalid,
    "invalid", Position, "position", UserInOut, "user in out")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::LoadStoreMode, Invalid,
    "invalid", B32, "b32", B64, "b64", B96, "b96", B128, "b128")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::MathFunc, Invalid,
    "invalid", Cos, "cos", Sin, "sin", Ex2, "ex2", Lg2, "lg2", Rcp, "rcp", Rsq,
    "rsq", Rcp64h, "rcp64h", Rsq64h, "rsq64h", Sqrt, "sqrt")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::ComparisonOperator,
    Invalid, "invalid", F, "f", Less, "less", Equal, "equal", Greater,
    "greater", LessEqual, "less equal", NotEqual, "not equal", GreaterEqual,
    "greater equal", Num, "num", Nan, "nan", LessU, "less U", EqualU, "equal U",
    LessEqualU, "less equal U", GreaterU, "greater U", NotEqualU, "not equal U",
    GreaterEqualU, "greater equal U", T, "t")

ENABLE_ENUM_FORMATTING(
    Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::BinaryOperator,
    Invalid, "invalid", And, "and", Or, "or", Xor, "xor")

#include "common/logging/log.hpp"
