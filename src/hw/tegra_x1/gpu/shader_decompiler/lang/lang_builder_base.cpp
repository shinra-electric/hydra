#include "hw/tegra_x1/gpu/shader_decompiler/lang/lang_builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang {

void LangBuilderBase::Start() {
    EmitHeader();
    WriteNewline();

    EmitTypeAliases();
    WriteNewline();

    // TODO: main function declaration
    EnterScope("");

    // Declare registers
    EnterScope("union");
    Write("i32 i;");
    Write("u32 u;");
    Write("f32 f;");
    ExitScope("regs[256]");
    WriteNewline();
}

void LangBuilderBase::Finish() {
    ExitScopeEmpty();
    ASSERT_DEBUG(indent == 0, ShaderDecompiler,
                 "Scope not fully exited (indentation: {})", indent);

    // TODO: avoid copying
    out_code.resize(code_str.size());
    std::copy(code_str.begin(), code_str.end(), out_code.begin());

    // Debug
    LOG_DEBUG(ShaderDecompiler, "Decompiled: \"\n{}\"", code_str);
}

void LangBuilderBase::OpMove(reg_t dst, u32 value) {
    WriteStatement("{} = 0x{:08x}", GetReg(dst, true), value);
}

void LangBuilderBase::OpLoad(reg_t dst, reg_t src, u64 imm) {
    // TODO: support indexing with src
    ASSERT_DEBUG(src == RZ, ShaderDecompiler,
                 "Indexing not implemented (src: r{})", src);

    WriteStatement("{} = {}", GetReg(dst, true),
                   GetSVNameQualified(GetSVFromAddr(imm), false));
}

void LangBuilderBase::OpStore(reg_t src, reg_t dst, u64 imm) {
    // TODO: support indexing with src
    ASSERT_DEBUG(dst == RZ, ShaderDecompiler,
                 "Indexing not implemented (dst: r{})", dst);

    WriteStatement("{} = {}", GetSVNameQualified(GetSVFromAddr(imm), true),
                   GetReg(src, false));
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang
