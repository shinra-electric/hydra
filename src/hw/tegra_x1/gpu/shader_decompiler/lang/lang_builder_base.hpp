#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang {

class LangBuilderBase : public BuilderBase {
  public:
    LangBuilderBase(std::vector<u8>& out_code_) : out_code{out_code_} {}

    void Start() override;
    void Finish() override;

  protected:
    virtual void EmitHeader() = 0;

    void Write(const std::string& str);
    void WriteNewline() { code_str += "\n"; }
    void WriteStatement(const std::string& str) { Write(str + "\n"); }

  private:
    std::vector<u8>& out_code;
    std::string code_str;

    u32 indent = 0;

    void EnterScope();
    void ExitScope();
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang
