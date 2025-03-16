#include "hw/tegra_x1/gpu/shader_decompiler/lang/lang_builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang {

void LangBuilderBase::Start() {
    EmitHeader();
    WriteNewline();

    // TODO: main function declaration
    EnterScope();
}

void LangBuilderBase::Finish() {
    ExitScope();

    // TODO: avoid copying
    out_code.resize(code_str.size());
    std::copy(code_str.begin(), code_str.end(), out_code.begin());

    // Debug
    LOG_DEBUG(ShaderDecompiler, "Decompiled: \"\n{}\"", code_str);
}

void LangBuilderBase::Write(const std::string& str) {
    for (u32 i = 0; i < indent; i++) {
        code_str += "    ";
    }
    code_str += str;
    code_str += "\n";
}

void LangBuilderBase::EnterScope() {
    Write("{");
    indent++;
}

void LangBuilderBase::ExitScope() {
    ASSERT_DEBUG(indent != 0, ShaderDecompiler,
                 "Cannot exit scope when indentation is 0");
    indent--;
    Write("}");
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang
