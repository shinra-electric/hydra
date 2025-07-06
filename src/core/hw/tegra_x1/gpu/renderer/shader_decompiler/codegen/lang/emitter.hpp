#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/codegen/emitter.hpp"

#define WRITE_ARGS fmt::format_string<T...> f, T &&... args
#define FMT fmt::format(f, std::forward<T>(args)...)

#define COMPONENT_STR(component) ("xyzw"[component])

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer {
struct CfgNode;
}

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::codegen::lang {

#define INVALID_VALUE "INVALID"

class LangEmitter : public Emitter {
  public:
    using Emitter::Emitter;

  protected:
    void Start() override;
    void Finish() override;

    // Emit

    virtual void EmitHeader() = 0;
    virtual void EmitTypeAliases() = 0;
    virtual void EmitDeclarations() = 0;
    virtual void EmitMainPrototype() = 0;
    virtual void EmitExitReturn() = 0;

    void EmitMainFunctionPrologue();

    void EmitFunction(const ir::Function& func) override;
    void EmitNode(const ir::Function& func, const analyzer::CfgNode* node);

    // Basic
    void EmitCopy(const ir::Value& dst, const ir::Value& src) override;
    void EmitNeg(const ir::Value& dst, const ir::Value& src) override;
    void EmitNot(const ir::Value& dst, const ir::Value& src) override;
    void EmitAdd(const ir::Value& dst, const ir::Value& srcA,
                 const ir::Value& srcB) override;
    void EmitMultiply(const ir::Value& dst, const ir::Value& srcA,
                      const ir::Value& srcB) override;
    void EmitFma(const ir::Value& dst, const ir::Value& srcA,
                 const ir::Value& srcB, const ir::Value& srcC) override;
    void EmitShiftLeft(const ir::Value& dst, const ir::Value& src,
                       u32 shift) override;
    void EmitShiftRight(const ir::Value& dst, const ir::Value& src,
                        u32 shift) override;
    void EmitCast(const ir::Value& dst, const ir::Value& src,
                  DataType dst_type) override;
    void EmitCompare(const ir::Value& dst, ComparisonOp op,
                     const ir::Value& srcA, const ir::Value& srcB) override;
    void EmitBitwise(const ir::Value& dst, BitwiseOp op, const ir::Value& srcA,
                     const ir::Value& srcB) override;
    void EmitSelect(const ir::Value& dst, const ir::Value& cond,
                    const ir::Value& src_true,
                    const ir::Value& src_false) override;

    // Control flow
    void EmitBranch(label_t target) override;
    void EmitBranchConditional(const ir::Value& cond, label_t target_true,
                               label_t target_false) override;
    void EmitBeginIf(const ir::Value& cond) override;
    void EmitEndIf() override;

    // Math
    void EmitMin(const ir::Value& dst, const ir::Value& srcA,
                 const ir::Value& srcB) override;
    void EmitMax(const ir::Value& dst, const ir::Value& srcA,
                 const ir::Value& srcB) override;
    void EmitMathFunction(const ir::Value& dst, MathFunc func,
                          const ir::Value& src) override;

    // Vector
    void EmitVectorExtract(const ir::Value& dst, const ir::Value& src,
                           u32 index) override;
    void EmitVectorInsert(const ir::Value& dst, const ir::Value& src,
                          u32 index) override;
    void EmitVectorConstruct(const ir::Value& dst, DataType data_type,
                             const std::vector<ir::Value>& elements) override;

    // Special
    void EmitExit() override;

    // Helpers
    template <typename... T>
    void WriteRaw(WRITE_ARGS) {
        code_str += FMT;
    }

    template <typename... T>
    void WriteWithIndent(WRITE_ARGS) {
        WriteRaw("{:{}}{}", "", indent * 4, FMT);
    }

    template <typename... T>
    void Write(WRITE_ARGS) {
        WriteWithIndent("{}\n", FMT);
    }

    void WriteNewline() { code_str += "\n"; }

    template <typename... T>
    void WriteStatement(WRITE_ARGS) {
        Write("{};", FMT);
    }

    template <typename... T>
    void EnterScope(WRITE_ARGS) {
        EnterScopeImpl("{} ", FMT);
    }

    void EnterScopeEmpty() { EnterScopeImpl(""); }

    template <typename... T>
    void ExitScope(WRITE_ARGS) {
        ExitScopeImpl(" {};", FMT);
    }

    void ExitScopeEmpty(bool semicolon = false) {
        if (semicolon)
            ExitScopeImpl(";");
        else
            ExitScopeImpl("");
    }

    template <typename T>
    std::string GetImmediateStr(const T imm) {
        if constexpr (std::is_same_v<T, i32>)
            return fmt::format("({:#x})", imm);
        if constexpr (std::is_same_v<T, i64>)
            return fmt::format("({:#x}ll)", imm);
        else if constexpr (std::is_same_v<T, u32>)
            return fmt::format("({:#x}u)", imm);
        else if constexpr (std::is_same_v<T, u64>)
            return fmt::format("({:#x}ull)", imm);
        else if constexpr (std::is_same_v<T, f32>)
            return fmt::format("({:#}f)", imm);
        else if constexpr (std::is_same_v<T, f64>)
            return fmt::format("({:#}f)", imm);
        else if constexpr (std::is_same_v<T, bool>)
            return fmt::format("{}", imm);
        else {
            LOG_ERROR(ShaderDecompiler, "Invalid immediate type {}",
                      typeid(T).name());
            return INVALID_VALUE;
        }
    }

    std::string GetImmediateStr(const u32 imm, DataType data_type) {
        switch (data_type) {
        case DataType::U8:
            return GetImmediateStr<u8>(imm & 0xff);
        case DataType::U16:
            return GetImmediateStr<u16>(imm & 0xffff);
        case DataType::U32:
            return GetImmediateStr<u32>(imm);
        case DataType::I8:
            return GetImmediateStr<i8>(std::bit_cast<i8>((u8)(imm & 0xff)));
        case DataType::I16:
            return GetImmediateStr<i16>(
                std::bit_cast<i16>((u16)(imm & 0xffff)));
        case DataType::I32:
            return GetImmediateStr<i32>(std::bit_cast<i32>(imm));
        case DataType::F16:
            return fmt::format("as_type<f16>((u16)0x{:04x})",
                               (u16)(imm & 0xffff));
        case DataType::F32:
            return GetImmediateStr<f32>(std::bit_cast<f32>(imm));
        // TODO: F16X2
        default:
            return INVALID_VALUE;
        }
    }

    std::string GetLocalStr(local_t local) { return locals.at(local); }

    template <bool load = true>
    std::string GetRegisterStr(reg_t reg, DataType data_type = DataType::U32) {
        if (load && reg == RZ)
            return GetImmediateStr(0, data_type);

        return fmt::format("r[{}].{}", u32(reg), GetTypeSuffixStr(data_type));
    }

    template <bool load = true>
    std::string GetPredicateStr(pred_t pred) {
        if (load && pred == PT)
            return GetImmediateStr(true);

        return fmt::format("p[{}]", u32(pred));
    }

    std::string GetAttrMemoryStr(const AMem amem,
                                 DataType data_type = DataType::U32) {
        // TODO: support indexing with reg
        return fmt::format("a[{} + 0x{:08x}].{}", GetRegisterStr(amem.reg),
                           amem.imm / sizeof(u32), GetTypeSuffixStr(data_type));
    }

    std::string GetConstMemoryStr(const CMem cmem,
                                  DataType data_type = DataType::U32) {
        return fmt::format("c[{}][{} + 0x{:08x}].{}", cmem.idx,
                           GetRegisterStr(cmem.reg), cmem.imm / sizeof(u32),
                           GetTypeSuffixStr(data_type));
    }

    std::string GetValueStr(const ir::Value& value, bool load = true) {
        switch (value.GetType()) {
        case ir::ValueType::Immediate:
            return GetImmediateStr(value.GetImmediate(), value.GetDataType());
        case ir::ValueType::Local:
            return GetLocalStr(value.GetLocal());
        case ir::ValueType::Register:
            return GetRegisterStr(value.GetRegister(), value.GetDataType());
        case ir::ValueType::Predicate:
            return GetPredicateStr(value.GetPredicate());
        case ir::ValueType::AttrMemory:
            return GetAttrMemoryStr(value.GetAttrMemory(), value.GetDataType());
        case ir::ValueType::ConstMemory:
            return GetConstMemoryStr(value.GetConstMemory(),
                                     value.GetDataType());
        default:
            LOG_FATAL(ShaderDecompiler, "Invalid value type {} for src",
                      value.GetType());
        }
    }

    template <typename... T>
    void StoreValue(const ir::Value& dst, WRITE_ARGS) {
        switch (dst.GetType()) {
        case ir::ValueType::Local:
            locals[dst.GetLocal()] = FMT;
            break;
        case ir::ValueType::Register:
            WriteStatement(
                "{} = {}",
                GetRegisterStr<false>(dst.GetRegister(), dst.GetDataType()),
                FMT);
            break;
        case ir::ValueType::Predicate:
            WriteStatement("{} = {}",
                           GetPredicateStr<false>(dst.GetPredicate()), FMT);
            break;
        case ir::ValueType::AttrMemory:
            WriteStatement(
                "{} = {}",
                GetAttrMemoryStr(dst.GetAttrMemory(), dst.GetDataType()), FMT);
            break;
        default:
            LOG_FATAL(ShaderDecompiler, "Invalid value type {} for dst",
                      dst.GetType());
        }
    }

    const char GetComponentStrFromIndex(u8 component_index) {
        ASSERT_DEBUG(component_index < 4, ShaderDecompiler,
                     "Invalid component index {}", component_index);

        return "xyzw"[component_index];
    }

    const std::string GetTypeSuffixStr(DataType data_type) {
        switch (data_type) {
        case DataType::U8:
            return "_u8";
        case DataType::U16:
            return "_u16";
        case DataType::U32:
            return "_u32";
        case DataType::I8:
            return "_i8";
        case DataType::I16:
            return "_i16";
        case DataType::I32:
            return "_i32";
        case DataType::F16:
            return "_f16";
        case DataType::F32:
            return "_f32";
        case DataType::F16X2:
            return "_f16x2";
        default:
            LOG_ERROR(ShaderDecompiler, "Invalid data type {}", data_type);
            return INVALID_VALUE;
        }
    }

    std::string GetMathFuncStr(MathFunc func) {
        // TODO: check
        switch (func) {
        case MathFunc::Cos:
            return "cos";
        case MathFunc::Sin:
            return "sin";
        case MathFunc::Ex2:
            return "exp2";
        case MathFunc::Lg2:
            return "log2";
        case MathFunc::Rcp:
            return "1.0f / ";
        case MathFunc::Rsq:
            return "1.0f / sqrt"; // TODO: isn's there a better way?
        case MathFunc::Rcp64h:
            return "1.0 / "; // TODO: correct?
        case MathFunc::Rsq64h:
            return "1.0 / sqrt"; // TODO: correct?
        case MathFunc::Sqrt:
            return "sqrt";
        default:
            return INVALID_VALUE;
        }
    }

    // template <typename... T>
    // std::string GetQualifiedSVName(const SV sv, bool output, WRITE_ARGS)
    // {
    //     // TODO: support qualifiers before the name as well
    //     return fmt::format("{} {}", FMT, GetSVQualifierName(sv, output));
    // }

    std::string GetSvStr(const Sv& sv) {
        switch (sv.semantic) {
        case SvSemantic::Position:
            return "position";
        case SvSemantic::UserInOut:
            return fmt::format("user{}", sv.index);
        default:
            LOG_NOT_IMPLEMENTED(ShaderDecompiler, "SV {} (index: {})",
                                sv.semantic, sv.index);
            return INVALID_VALUE;
        }
    }

    std::string GetSvAccessQualifiedStr(const SvAccess& sv_access,
                                        bool output) {
        // TODO: is it okay to access components just like this?
        return fmt::format("{}.{}.{}", GetInOutStr(output),
                           GetSvStr(sv_access.sv),
                           GetComponentStrFromIndex(sv_access.component_index));
    }

    std::string GetInOutStr(bool output) { return (output ? "__out" : "__in"); }

  private:
    std::string code_str;

    std::map<local_t, std::string> locals;
    u32 indent{0};

    template <typename... T>
    void EnterScopeImpl(WRITE_ARGS) {
        Write("{}{{", FMT);
        indent++;
    }

    template <typename... T>
    void ExitScopeImpl(WRITE_ARGS) {
        ASSERT_DEBUG(indent != 0, ShaderDecompiler,
                     "Cannot exit scope when indentation is 0");
        indent--;
        Write("}}{}", FMT);
    }
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::codegen::lang

#undef FMT
#undef WRITE_ARGS
