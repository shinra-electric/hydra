#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/codegen/lang/emitter.hpp"

#include "core/hw/tegra_x1/gpu/renderer/shader_cache.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/memory_analyzer.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::codegen::lang {

namespace {

std::string cmp_op_to_str(ComparisonOp cmp) {
    // TODO: are the U ones the same?
    switch (cmp) {
    case ComparisonOp::F:
        return INVALID_VALUE; // TODO
    case ComparisonOp::Less:
    case ComparisonOp::LessU:
        return "<";
    case ComparisonOp::Equal:
    case ComparisonOp::EqualU:
        return "==";
    case ComparisonOp::LessEqual:
    case ComparisonOp::LessEqualU:
        return "<=";
    case ComparisonOp::Greater:
    case ComparisonOp::GreaterU:
        return ">";
    case ComparisonOp::NotEqual:
    case ComparisonOp::NotEqualU:
        return "!=";
    case ComparisonOp::GreaterEqual:
    case ComparisonOp::GreaterEqualU:
        return ">=";
    case ComparisonOp::Num:
        return INVALID_VALUE; // TODO
    case ComparisonOp::Nan:
        return INVALID_VALUE; // TODO
    case ComparisonOp::T:
        return INVALID_VALUE; // TODO
    default:
        return INVALID_VALUE;
    }
}

std::string bit_op_to_str(BitwiseOp bin) {
    switch (bin) {
    case BitwiseOp::And:
        return "&";
    case BitwiseOp::Or:
        return "|";
    case BitwiseOp::Xor:
        return "^";
    default:
        return INVALID_VALUE;
    }
}

} // namespace

void LangEmitter::Start() {
    // Header
    EmitHeader();
    WriteNewline();

    // Type aliases
    EmitTypeAliases();
    WriteNewline();

    // Reg type
    EnterScope("union Reg");
    Write("u8 _u8;");
    Write("u16 _u16;");
    Write("u32 _u32;");
    Write("i8 _i8;");
    Write("i16 _i16;");
    Write("i32 _i32;");
    Write("f16 _f16;");
    Write("f32 _f32;");
    Write("half2 _f16x2;");
    ExitScopeEmpty(true);
    WriteNewline();

    // Declarations
    EmitDeclarations();

    // Main prototype
    EmitMainPrototype();

    // Temporary
    EnterScope("union");
    Write("int4 i;");
    Write("uint4 u;");
    Write("float4 f;");
    ExitScope("temp");
    WriteNewline();

    // Registers
    Write("Reg r[256];");
    WriteNewline();

    // Predicates
    Write("bool p[8];"); // TODO: is the size correct?
    WriteNewline();

    // Attribute memory
    Write("Reg a[0x200];"); // TODO: what should the size be?
    WriteNewline();

    // Inputs
    // TODO: these are provided in the shader header, no need for analysis
    switch (context.type) {
    case ShaderType::Vertex:
        for (u32 i = 0; i < VERTEX_ATTRIB_COUNT; i++) {
            const auto vertex_attrib_state = state.vertex_attrib_states[i];
            if (vertex_attrib_state.type == engines::VertexAttribType::None)
                continue;

            // HACK: how are attributes disabled?
            if (vertex_attrib_state.is_fixed)
                continue;

            // TODO: only set if the Rendered backend doesn't support scaled
            // attributes
            bool needs_scaling = (vertex_attrib_state.type ==
                                      engines::VertexAttribType::Sscaled ||
                                  vertex_attrib_state.type ==
                                      engines::VertexAttribType::Uscaled);

            const auto sv = Sv(SvSemantic::UserInOut, i);
            for (u32 c = 0; c < 4; c++) {
                const auto attr =
                    GetAttrMemoryStr({RZ, 0x80 + i * 0x10 + c * 0x4});
                const auto qualified_name =
                    GetSvAccessQualifiedStr(SvAccess(sv, c), false);
                if (needs_scaling)
                    WriteStatement("{} = as_type<uint>((float){})", attr,
                                   qualified_name);
                else
                    WriteStatement("{} = as_type<uint>({})", attr,
                                   qualified_name);
            }
        }
        break;
    case ShaderType::Fragment:
#define ADD_INPUT(sv_semantic, index, a_base)                                  \
    {                                                                          \
        for (u32 c = 0; c < 4; c++) {                                          \
            WriteStatement("{} = as_type<uint>({})",                           \
                           GetAttrMemoryStr({RZ, a_base + c * 0x4}),  \
                           GetSvAccessQualifiedStr(SvAccess(Sv(sv_semantic, index), c), false));   \
        }                                                                      \
    }

        ADD_INPUT(SvSemantic::Position, invalid<u8>(), 0x70);
        for (const auto input : memory_analyzer.GetStageInputs())
            ADD_INPUT(SvSemantic::UserInOut, input, 0x80 + input * 0x10);

#undef ADD_INPUT
        break;
    default:
        break;
    }
    WriteNewline();

    // Constant memory
    Write("Reg c[{}][0x40];",
          CONST_BUFFER_BINDING_COUNT); // TODO: what should the size be?
    WriteNewline();

    // Uniform buffers
    for (const auto& [index, size] : memory_analyzer.GetUniformBuffers()) {
        u32 u32_count = size / sizeof(u32);
        for (u32 i = 0; i < u32_count; i++)
            WriteStatement("{} = ubuff{}.data[{}]",
                           GetConstMemoryStr({index, RZ, i * sizeof(u32)}), index,
                           i);
    }
    WriteNewline();
}

void LangEmitter::Finish() {
    // End
    ExitScopeEmpty();
    ASSERT_DEBUG(indent == 0, ShaderDecompiler,
                 "Scope not fully exited (indentation: {})", indent);

    // TODO: footer

    // TODO: avoid copying
    out_code.resize(code_str.size());
    std::copy(code_str.begin(), code_str.end(), out_code.begin());

    // Debug
    LOG_DEBUG(ShaderDecompiler, "decompiled: \"\n{}\"", code_str);
}

void LangEmitter::EmitFunction(const ir::Function& func) {
    // TODO
    abort();
}

void LangEmitter::EmitCopy(const ir::Value& dst, const ir::Value& src) {
    StoreValue(dst, "{}", GetValueStr(src));
}

void LangEmitter::EmitNeg(const ir::Value& dst, const ir::Value& src) {
    StoreValue(dst, "(-{})", GetValueStr(src));
}

void LangEmitter::EmitNot(const ir::Value& dst, const ir::Value& src) {
    StoreValue(dst, "(!{})", GetValueStr(src));
}

void LangEmitter::EmitAdd(const ir::Value& dst, const ir::Value& srcA,
                          const ir::Value& srcB) {
    StoreValue(dst, "({} + {})", GetValueStr(srcA), GetValueStr(srcB));
}

void LangEmitter::EmitMultiply(const ir::Value& dst, const ir::Value& srcA,
                               const ir::Value& srcB) {
    StoreValue(dst, "({} * {})", GetValueStr(srcA), GetValueStr(srcB));
}

void LangEmitter::EmitFma(const ir::Value& dst, const ir::Value& srcA,
                          const ir::Value& srcB, const ir::Value& srcC) {
    StoreValue(dst, "({} * {} + {})", GetValueStr(srcA), GetValueStr(srcB),
               GetValueStr(srcC));
}

void LangEmitter::EmitShiftLeft(const ir::Value& dst, const ir::Value& src,
                                u32 shift) {
    StoreValue(dst, "({} << {})", GetValueStr(src), shift);
}

void LangEmitter::EmitShiftRight(const ir::Value& dst, const ir::Value& src,
                                 u32 shift) {
    StoreValue(dst, "({} >> {})", GetValueStr(src), shift);
}

void LangEmitter::EmitCast(const ir::Value& dst, const ir::Value& src,
                           DataType dst_type) {
    StoreValue(dst, "({}({}))", dst_type, GetValueStr(src));
}

void LangEmitter::EmitCompare(const ir::Value& dst, ComparisonOp op,
                              const ir::Value& srcA, const ir::Value& srcB) {
    StoreValue(dst, "({} {} {})", GetValueStr(srcA), cmp_op_to_str(op),
               GetValueStr(srcB));
}

void LangEmitter::EmitBitwise(const ir::Value& dst, BitwiseOp op,
                              const ir::Value& srcA, const ir::Value& srcB) {
    StoreValue(dst, "({} {} {})", GetValueStr(srcA), bit_op_to_str(op),
               GetValueStr(srcB));
}

void LangEmitter::EmitSelect(const ir::Value& dst, const ir::Value& cond,
                             const ir::Value& src_true,
                             const ir::Value& src_false) {
    StoreValue(dst, "({} ? {} : {})", GetValueStr(cond), GetValueStr(src_true),
               GetValueStr(src_false));
}

void LangEmitter::EmitBranch(label_t target) {
    LOG_FATAL(ShaderDecompiler, "Should not happen");
}

void LangEmitter::EmitBranchConditional(const ir::Value& cond,
                                        label_t target_true,
                                        label_t target_false) {
    LOG_FATAL(ShaderDecompiler, "Should not happen");
}

void LangEmitter::EmitBeginIf(const ir::Value& cond) {
    EnterScope("if ({})", GetValueStr(cond));
}

void LangEmitter::EmitEndIf() { ExitScopeEmpty(); }

void LangEmitter::EmitMin(const ir::Value& dst, const ir::Value& srcA,
                          const ir::Value& srcB) {
    StoreValue(dst, "min({}, {})", GetValueStr(srcA), GetValueStr(srcB));
}

void LangEmitter::EmitMax(const ir::Value& dst, const ir::Value& srcA,
                          const ir::Value& srcB) {
    StoreValue(dst, "max({}, {})", GetValueStr(srcA), GetValueStr(srcB));
}

void LangEmitter::EmitMathFunction(const ir::Value& dst, MathFunc func,
                                   const ir::Value& src) {
    StoreValue(dst, "({}({}))", GetMathFuncStr(func), GetValueStr(src));
}

void LangEmitter::EmitVectorExtract(const ir::Value& dst, const ir::Value& src,
                                    u32 index) {
    StoreValue(dst, "({}.{})", GetValueStr(src),
               GetComponentStrFromIndex(index));
}

void LangEmitter::EmitVectorInsert(const ir::Value& dst, const ir::Value& src,
                                   u32 index) {
    WriteStatement("{}.{} = {}", GetValueStr(dst),
                   GetComponentStrFromIndex(index), GetValueStr(src));
}

void LangEmitter::EmitVectorConstruct(const ir::Value& dst, DataType data_type,
                         const std::vector<ir::Value>& elements) {
    StoreValue(dst, "({})", fmt::join(elements, ", "));
}

void LangEmitter::EmitExit() {
    // Outputs
    switch (context.type) {
    case ShaderType::Vertex:
        // TODO: don't hardcode the bit cast type
#define ADD_OUTPUT(sv_semantic, index, a_base)                                 \
    {                                                                          \
        for (u32 c = 0; c < 4; c++) {                                          \
            WriteStatement("{} = as_type<float>({})",                          \
                           GetSvAccessQualifiedStr(                           \
                               SvAccess(Sv(sv_semantic, index), c), true),     \
                           GetAttrMemoryStr({RZ, a_base + c * 0x4}));  \
        }                                                                      \
    }

        ADD_OUTPUT(SvSemantic::Position, invalid<u8>(), 0x70);
        for (const auto output : memory_analyzer.GetStageOutputs())
            ADD_OUTPUT(SvSemantic::UserInOut, output, 0x80 + output * 0x10);

#undef ADD_OUTPUT
        break;
    case ShaderType::Fragment:
        for (u32 i = 0; i < COLOR_TARGET_COUNT; i++) {
            const auto color_target_format = state.color_target_formats[i];
            if (color_target_format == TextureFormat::Invalid)
                continue;

            for (u32 c = 0; c < 4; c++) {
                WriteStatement(
                    "{} = as_type<{}>({})",
                    GetSvAccessQualifiedStr(
                        SvAccess(Sv(SvSemantic::UserInOut, i), c), true),
                    to_data_type(color_target_format),
                    GetRegisterStr(i * 4 + c));
            }
        }
        break;
    default:
        break;
    }
    WriteNewline();

    EmitExitReturn();
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::Lang
