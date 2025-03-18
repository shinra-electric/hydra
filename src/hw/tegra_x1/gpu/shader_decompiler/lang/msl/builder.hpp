#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/lang/lang_builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL {

class Builder final : public LangBuilderBase {
  public:
    Builder(const Analyzer& analyzer, const Renderer::ShaderType type,
            const GuestShaderState& state, std::vector<u8>& out_code)
        : LangBuilderBase(analyzer, type, state, out_code) {}

  protected:
    void EmitHeader() override;
    void EmitTypeAliases() override;

    std::string GetSVQualifierName(const SV sv, bool output) override;
    std::string GetStageInQualifierName() override { return "[[stage_in]]"; }
    std::string GetUniformBufferQualifierName(const u32 index) override {
        return fmt::format("[[buffer({})]]", index);
    }
    std::string GetTextureQualifierName(const u32 index) override {
        return fmt::format("[[texture({})]]", index);
    }
    std::string GetSamplerQualifierName(const u32 index) override {
        return fmt::format("[[sampler({})]]", index);
    }
    std::string GetStageQualifierName() override;

    std::string EmitTextureSample(u32 index,
                                  const std::string& coords) override;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL
