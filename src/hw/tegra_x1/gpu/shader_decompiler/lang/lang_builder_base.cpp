#include "hw/tegra_x1/gpu/shader_decompiler/lang/lang_builder_base.hpp"

#include "hw/tegra_x1/gpu/shader_cache.hpp"
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
    Write("int i;");
    Write("uint u;");
    Write("float f;");
    ExitScope("r[256]");
    WriteNewline();

    // Temporary
    EnterScope("union");
    Write("int4 i;");
    Write("uint4 u;");
    Write("float4 f;");
    ExitScope("temp");
    WriteNewline();

    // A memory
    Write("uint a[0x200];"); // TODO: what should the size be?
    WriteNewline();

    // Inputs
    switch (type) {
    case Renderer::ShaderType::Vertex:
        for (u32 i = 0; i < VERTEX_ATTRIB_COUNT; i++) {
            const auto vertex_attrib_state = state.vertex_attrib_states[i];
            if (vertex_attrib_state.type == Engines::VertexAttribType::None)
                continue;

            // HACK: how are attributes disabled?
            if (vertex_attrib_state.is_fixed)
                continue;

            const auto sv = SV(SVSemantic::UserInOut, i);
            for (u32 c = 0; c < 4; c++) {
                WriteStatement(
                    "{} = as_type<uint>({})",
                    GetA("0x{:08x}", 0x80 + i * 0x10 + c * 0x4),
                    GetSVNameQualified(SV(SVSemantic::UserInOut, i, c), false));
            }
        }
        break;
    case Renderer::ShaderType::Fragment:
#define ADD_INPUT(sv_semantic, index, a_base)                                  \
    {                                                                          \
        for (u32 c = 0; c < 4; c++) {                                          \
            WriteStatement(                                                    \
                "{} = as_type<uint>({})", GetA("0x{:08x}", a_base + c * 0x4),  \
                GetSVNameQualified(SV(sv_semantic, index, c), false));         \
        }                                                                      \
    }

        ADD_INPUT(SVSemantic::Position, invalid<u8>(), 0x70);
        for (const auto input : analyzer.GetStageInputs())
            ADD_INPUT(SVSemantic::UserInOut, input, 0x80 + input * 0x10);

#undef ADD_INPUT
        break;
    default:
        break;
    }
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

void LangBuilderBase::OpExit() {
    // Outputs
    switch (type) {
    case Renderer::ShaderType::Vertex:
        // TODO: don't hardcode the bit cast type
#define ADD_OUTPUT(sv_semantic, index, a_base)                                 \
    {                                                                          \
        for (u32 c = 0; c < 4; c++) {                                          \
            WriteStatement(                                                    \
                "{} = as_type<float>({})",                                     \
                GetSVNameQualified(SV(sv_semantic, index, c), true),           \
                GetA("0x{:08x}", a_base + c * 0x4));                           \
        }                                                                      \
    }

        ADD_OUTPUT(SVSemantic::Position, invalid<u8>(), 0x70);
        for (const auto output : analyzer.GetStageOutputs())
            ADD_OUTPUT(SVSemantic::UserInOut, output, 0x80 + output * 0x10);

#undef ADD_OUTPUT
        break;
    case Renderer::ShaderType::Fragment:
        for (u32 i = 0; i < COLOR_TARGET_COUNT; i++) {
            const auto color_target_format = state.color_target_formats[i];
            if (color_target_format == Renderer::TextureFormat::Invalid)
                continue;

            for (u32 c = 0; c < 4; c++) {
                WriteStatement(
                    "{} = as_type<{}>({})",
                    GetSVNameQualified(SV(SVSemantic::UserInOut, i, c), true),
                    to_data_type(color_target_format),
                    GetReg(i * 4 + c, false));
            }
        }
        break;
    default:
        break;
    }
    WriteNewline();

    // Return
    WriteStatement("return __out");
}

void LangBuilderBase::OpMove(reg_t dst, reg_t src) {
    WriteStatement("{} = {}", GetReg(dst, true), GetReg(src, false));
}

void LangBuilderBase::OpMoveImmediate(reg_t dst, u32 value) {
    WriteStatement("{} = 0x{:08x}", GetReg(dst, true), value);
}

void LangBuilderBase::OpLoad(reg_t dst, reg_t src, u64 imm) {
    // TODO: support indexing with src
    WriteStatement("{} = {}", GetReg(dst, true), GetA("0x{:08x}", imm));
}

void LangBuilderBase::OpStore(reg_t src, reg_t dst, u64 imm) {
    // TODO: support indexing with src
    WriteStatement("{} = {}", GetA("0x{:08x}", imm), GetReg(src, false));
}

void LangBuilderBase::OpInterpolate(reg_t dst, reg_t src, u64 imm) {
    // TODO: support indexing with src
    WriteStatement("{} = {}", GetReg(dst, true), GetA("0x{:08x}", imm));
}

void LangBuilderBase::OpTextureSample(reg_t dst, u32 index, reg_t coords) {
    EmitReadToTemp(coords, 2);
    WriteStatement("temp.f = {}", EmitTextureSample(index, "temp.f.xy"));
    EmitWriteFromTemp(dst);
}

std::string LangBuilderBase::GetMainArgs() {
#define ADD_ARG(fmt, ...)                                                      \
    args += fmt::format(", {}", fmt::format(fmt, __VA_ARGS__))

    std::string args;
    // TODO: input SVs

    // Uniform buffers
    // TODO

    // Storage buffers
    // TODO

    // Textures
    for (const u32 index : analyzer.GetTextureSlots()) {
        // TODO: don't hardcode texture type
        args += fmt::format(", texture2d<float> tex{} {}", index,
                            GetTextureQualifierName(index));
        args += fmt::format(", sampler samplr{} {}", index,
                            GetSamplerQualifierName(index));
    }

    // Images
    // TODO

#undef ADD_ARG

    return args;
}

void LangBuilderBase::EmitStageInputs() {
    EnterScope("struct StageIn");

    // SVs
    // Handled in GetMainArgs

    // Stage inputs
    switch (type) {
    case Renderer::ShaderType::Vertex:
        for (u32 i = 0; i < VERTEX_ATTRIB_COUNT; i++) {
            const auto vertex_attrib_state = state.vertex_attrib_states[i];
            if (vertex_attrib_state.type == Engines::VertexAttribType::None)
                continue;

            // HACK: how are attributes disabled?
            if (vertex_attrib_state.is_fixed)
                continue;

            const auto sv = SV(SVSemantic::UserInOut, i);
            Write("{};",
                  GetQualifiedSVName(sv, false, "{}4 {}",
                                     to_data_type(vertex_attrib_state.type),
                                     GetSVName(sv)));
        }
        break;
    case Renderer::ShaderType::Fragment:
        Write("{};", GetQualifiedSVName(SVSemantic::Position, false,
                                        "float4 position"));
        for (const auto input : analyzer.GetStageInputs()) {
            const auto sv = SV(SVSemantic::UserInOut, input);
            // TODO: don't hardcode the type
            Write("{};",
                  GetQualifiedSVName(sv, false, "float4 {}", GetSVName(sv)));
        }
        break;
    default:
        break;
    }

    ExitScopeEmpty(true);
}

void LangBuilderBase::EmitStageOutputs() {
    EnterScope("struct StageOut");

    // SVs
    for (const auto sv_semantic : analyzer.GetOutputSVs()) {
        switch (sv_semantic) {
        default:
            LOG_NOT_IMPLEMENTED(ShaderDecompiler, "Output SV semantic {}",
                                sv_semantic);
            break;
        }
    }

    // Stage outputs
    switch (type) {
    case Renderer::ShaderType::Vertex:
        Write("{};", GetQualifiedSVName(SVSemantic::Position, true,
                                        "float4 position"));
        for (const auto output : analyzer.GetStageOutputs()) {
            const auto sv = SV(SVSemantic::UserInOut, output);
            // TODO: don't hardcode the type
            Write("{};",
                  GetQualifiedSVName(sv, true, "float4 {}", GetSVName(sv)));
        }
        break;
    case Renderer::ShaderType::Fragment:
        for (u32 i = 0; i < COLOR_TARGET_COUNT; i++) {
            const auto color_target_format = state.color_target_formats[i];
            if (color_target_format == Renderer::TextureFormat::Invalid)
                continue;

            const auto sv = SV(SVSemantic::UserInOut, i);
            Write("{};", GetQualifiedSVName(sv, true, "{}4 {}",
                                            to_data_type(color_target_format),
                                            GetSVName(sv)));
        }
        break;
    default:
        break;
    }

    ExitScopeEmpty(true);
}

void LangBuilderBase::EmitReadToTemp(reg_t src, u32 count) {
    for (u32 i = 0; i < count; i++) {
        WriteStatement("temp.u.{} = {}", GetComponentFromIndex(i),
                       GetReg(src + i, false));
    }
}

void LangBuilderBase::EmitWriteFromTemp(reg_t dst, u32 count) {
    for (u32 i = 0; i < count; i++) {
        WriteStatement("{} = temp.u.{}", GetReg(dst + i, true),
                       GetComponentFromIndex(i));
    }
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang
