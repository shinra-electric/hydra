#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/lang/lang_builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL {

class Builder final : public LangBuilderBase {
  public:
    Builder(const Analyzer& analyzer, const Renderer::ShaderType type,
            std::vector<u8>& out_code)
        : LangBuilderBase(analyzer, type, out_code) {}

  protected:
    void EmitHeader() override;
    void EmitTypeAliases() override;

    std::string GetSVQualifierName(const SV sv, bool output) override;
    std::string GetStageInQualifierName() override { return "[[stage_in]]"; }
    std::string GetStageQualifierName() override;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL
