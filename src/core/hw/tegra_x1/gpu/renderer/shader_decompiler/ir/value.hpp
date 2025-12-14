#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/type.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {

enum class ValueKind {
    Undefined,
    RawValue,
    Immediate,
    Local,
    Register,
    Predicate,
    AttrMemory,
    ConstMemory,
    Label,
};

class Value {
  public:
    static Value Undefined() { return Value{ValueKind::Undefined}; }
    template <typename T>
    static Value RawValue(const T raw_value) {
        return Value{ValueKind::RawValue,
                     .raw_value = static_cast<u64>(raw_value)};
    }
    static Value Immediate(const u32 imm, const Type type = ScalarType::U32) {
        return Value{ValueKind::Immediate, type, .imm = imm};
    }
    static Value Local(const local_t local, const Type type = ScalarType::U32) {
        return Value{ValueKind::Local, type, .local = local};
    }
    static Value Register(const reg_t reg, const Type type = ScalarType::U32) {
        return Value{ValueKind::Register, type, .reg = reg};
    }
    static Value Predicate(const pred_t pred) {
        return Value{ValueKind::Predicate, ScalarType::Bool, .pred = pred};
    }
    static Value AttrMemory(const AMem& amem,
                            const Type type = ScalarType::U32) {
        return Value{ValueKind::AttrMemory, type, .amem = amem};
    }
    static Value ConstMemory(const CMem& cmem,
                             const Type type = ScalarType::U32) {
        return Value{ValueKind::ConstMemory, type, .cmem = cmem};
    }
    static Value Label(const label_t label) {
        return Value{ValueKind::Label, .label = label};
    }

    bool operator==(const Value& other) const {
        if (kind != other.kind)
            return false;

        switch (kind) {
        case ValueKind::Undefined:
            return true;
        case ValueKind::RawValue:
            return raw_value == other.raw_value;
        case ValueKind::Immediate:
            return imm == other.imm;
        case ValueKind::Local:
            return local == other.local;
        case ValueKind::Register:
            return reg == other.reg;
        case ValueKind::Predicate:
            return pred == other.pred;
        case ValueKind::AttrMemory:
            return amem == other.amem;
        case ValueKind::ConstMemory:
            return cmem == other.cmem;
        case ValueKind::Label:
            return label == other.label;
        }
    }

  public: // TODO: private?
    ValueKind kind;
    Type type;

    union {
        u64 raw_value;
        u32 imm;
        local_t local;
        reg_t reg;
        pred_t pred;
        AMem amem;
        CMem cmem;
        label_t label;
    };

    template <ValueKind kind_>
    void AssertKind() const {
        ASSERT_DEBUG(kind == kind_, ShaderDecompiler,
                     "Invalid value kind (expected {}, got {})", kind_, kind);
    }

  public:
    GETTER(kind, GetKind);
    GETTER(type, GetType);

    template <typename T>
    T GetRawValue() const {
        AssertKind<ValueKind::RawValue>();
        return static_cast<T>(raw_value);
    }
    u32 GetImmediate() const {
        AssertKind<ValueKind::Immediate>();
        return imm;
    }
    local_t GetLocal() const {
        AssertKind<ValueKind::Local>();
        return local;
    }
    reg_t GetRegister() const {
        AssertKind<ValueKind::Register>();
        return reg;
    }
    pred_t GetPredicate() const {
        AssertKind<ValueKind::Predicate>();
        return pred;
    }
    AMem GetAttrMemory() const {
        AssertKind<ValueKind::AttrMemory>();
        return amem;
    }
    CMem GetConstMemory() const {
        AssertKind<ValueKind::ConstMemory>();
        return cmem;
    }
    label_t GetLabel() const {
        AssertKind<ValueKind::Label>();
        return label;
    }
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir

ENABLE_ENUM_FORMATTING(
    hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind, Undefined,
    "undefined", RawValue, "raw value", Immediate, "immediate", Register,
    "register", Predicate, "predicate", AttrMemory, "attribute memory",
    ConstMemory, "constant memory", Label, "label")

template <>
struct fmt::formatter<
    hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::Value>
    : formatter<string_view> {
    template <typename FormatContext>
    auto
    format(const hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::Value&
               value,
           FormatContext& ctx) const {
        std::string str;
        switch (value.GetKind()) {
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            Undefined:
            str = "undefined";
            break;
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            RawValue:
            // TODO: figure out a better way to print this
            str = fmt::format("0x{:x}", value.GetRawValue<hydra::u64>());
            break;
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            Immediate:
            str = fmt::format("0x{:08x}", value.GetImmediate());
            break;
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            Local:
            str = fmt::format("{}", value.GetLocal());
            break;
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            Register:
            str = fmt::format("{}", value.GetRegister());
            break;
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            Predicate:
            str = fmt::format("{}", value.GetPredicate());
            break;
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            AttrMemory:
            str = fmt::format("{}", value.GetAttrMemory());
            break;
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            ConstMemory:
            str = fmt::format("{}", value.GetConstMemory());
            break;
        case hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir::ValueKind::
            Label:
            str = fmt::format("{}", value.GetLabel());
            break;
        }

        return formatter<string_view>::format(str, ctx);
    }
};
