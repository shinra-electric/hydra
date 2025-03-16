#include "hw/tegra_x1/gpu/shader_decompiler/lang/lang_builder_base.hpp"

#include "hw/tegra_x1/gpu/shader_decompiler/analyzer.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang {

void LangBuilderBase::Start() {
    // Header
    EmitHeader();
    WriteNewline();

    // Type aliases
    EmitTypeAliases();
    WriteNewline();

    // Declarations

    // Stage inputs
    EmitStageInputs();
    WriteNewline();

    // Stage outputs
    EmitStageOutputs();
    WriteNewline();

    // Main declaration
    EnterScope("{} StageOut main_(StageIn __in {}{})", GetStageQualifierName(),
               GetStageInQualifierName(), GetMainArgs());

    // Output
    Write("StageOut __out;");
    WriteNewline();

    // Registers
    EnterScope("union");
    Write("i32 i;");
    Write("u32 u;");
    Write("f32 f;");
    ExitScope("r[256]");
    WriteNewline();
}

void LangBuilderBase::Finish() {
    // End
    ExitScopeEmpty();
    ASSERT_DEBUG(indent == 0, ShaderDecompiler,
                 "Scope not fully exited (indentation: {})", indent);

    // TODO: footer

    // TODO: avoid copying
    out_code.resize(code_str.size());
    std::copy(code_str.begin(), code_str.end(), out_code.begin());

    // Debug
    LOG_DEBUG(ShaderDecompiler, "Decompiled: \"\n{}\"", code_str);
}

void LangBuilderBase::OpExit() { WriteStatement("return __out"); }

void LangBuilderBase::OpMove(reg_t dst, u32 value) {
    WriteStatement("{} = 0x{:08x}", GetReg(dst, true), value);
}

void LangBuilderBase::OpLoad(reg_t dst, reg_t src, u64 imm) {
    // TODO: support indexing with src
    WriteStatement("{} = {}", GetReg(dst, true),
                   GetSVNameQualified(GetSVFromAddr(imm), false));
}

void LangBuilderBase::OpStore(reg_t src, reg_t dst, u64 imm) {
    // TODO: support indexing with src
    WriteStatement("{} = {}", GetSVNameQualified(GetSVFromAddr(imm), true),
                   GetReg(src, false));
}

std::string LangBuilderBase::GetMainArgs() {
#define ADD_ARG(fmt, ...)                                                      \
    args += fmt::format(", {}", fmt::format(fmt, __VA_ARGS__))

    std::string args;
    // TODO: input SVs

#undef ADD_ARG

    return args;
}

void LangBuilderBase::EmitStageInputs() {
    EnterScope("struct StageIn");

    // SVs
    for (const auto sv_semantic : analyzer.GetInputSVs()) {
        switch (sv_semantic) {
        case SVSemantic::Position:
            Write("{};", GetQualifiedSVName(SVSemantic::Position, false,
                                            "float4 position"));
            break;
        default:
            break; // Handled in GetMainArgs
        }
    }

    // Stage inputs
    for (const auto input : analyzer.GetStageInputs()) {
        const auto sv = SV(SVSemantic::UserInOut, input);
        // TODO: don't hardcode the type
        Write("{};", GetQualifiedSVName(sv, false, "float3 {}", GetSVName(sv)));
    }

    ExitScopeEmpty(true);
}

void LangBuilderBase::EmitStageOutputs() {
    EnterScope("struct StageOut");

    // SVs
    for (const auto sv_semantic : analyzer.GetOutputSVs()) {
        switch (sv_semantic) {
        case SVSemantic::Position:
            Write("{};", GetQualifiedSVName(SVSemantic::Position, true,
                                            "float4 position"));
            break;
        default:
            LOG_NOT_IMPLEMENTED(ShaderDecompiler, "Output SV semantic {}",
                                sv_semantic);
            break;
        }
    }

    // Stage outputs
    for (const auto output : analyzer.GetStageOutputs()) {
        const auto sv = SV(SVSemantic::UserInOut, output);
        // TODO: don't hardcode the type
        Write("{};", GetQualifiedSVName(sv, true, "float3 {}", GetSVName(sv)));
    }

    ExitScopeEmpty(true);
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang
