#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/lang/lang_builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Lang::MSL {

class Builder final : public LangBuilderBase {
  public:
    Builder(const Analyzer& analyzer, const ShaderType type,
            const GuestShaderState& state, std::vector<u8>& out_code,
            ResourceMapping& out_resource_mapping)
        : LangBuilderBase(analyzer, type, state, out_code,
                          out_resource_mapping) {}

  protected:
    void InitializeResourceMapping() override;

    void EmitHeader() override;
    void EmitTypeAliases() override;
    void EmitDeclarations() override;
    void EmitMainPrototype() override;
    void EmitExit() override;

    std::string EmitTextureSample(u32 index,
                                  const std::string& coords) override;

  private:
    // Helpers
    std::string GetSVQualifierName(const SV sv, bool output);
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Lang::MSL
