#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir {

enum class ValueType {
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
    template <typename T>
    static Value RawValue(const T raw_value) {
        return Value{ValueType::RawValue,
                     .raw_value = static_cast<u64>(raw_value)};
    }
    static Value Immediate(const u32 imm,
                           const DataType data_type = DataType::U32) {
        return Value{ValueType::Immediate, data_type, .imm = imm};
    }
    static Value Local(const local_t local) {
        return Value{ValueType::Local, .local = local};
    }
    static Value Register(const reg_t reg,
                          const DataType data_type = DataType::U32) {
        return Value{ValueType::Register, data_type, .reg = reg};
    }
    static Value Predicate(const pred_t pred) {
        return Value{ValueType::Predicate, .pred = pred};
    }
    static Value AttrMemory(const AMem& amem,
                            const DataType data_type = DataType::U32) {
        return Value{ValueType::AttrMemory, data_type, .amem = amem};
    }
    static Value ConstMemory(const CMem& cmem,
                             const DataType data_type = DataType::U32) {
        return Value{ValueType::ConstMemory, data_type, .cmem = cmem};
    }
    static Value Label(const label_t label) {
        return Value{ValueType::Label, .label = label};
    }

  public: // TODO: private?
    ValueType type;
    DataType data_type{DataType::Invalid};

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

  public:
    GETTER(type, GetType);
    template <typename T>
    T GetRawValue() const {
        return static_cast<T>(raw_value);
    }
    GETTER(local, GetLocal);
    GETTER(imm, GetImmediate);
    GETTER(reg, GetRegister);
    GETTER(pred, GetPredicate);
    CONST_REF_GETTER(amem, GetAttributeMemory);
    CONST_REF_GETTER(cmem, GetConstMemory);
    GETTER(label, GetLabel);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::ir
