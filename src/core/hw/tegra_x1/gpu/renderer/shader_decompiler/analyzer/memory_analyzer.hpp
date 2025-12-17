#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/module.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer {

struct TextureInfo {
    TextureType type;
    bool is_depth;
};

class MemoryAnalyzer {
  public:
    void Analyze(const ir::Module& modul);

  private:
    std::vector<SvSemantic> input_svs;
    std::vector<SvSemantic> output_svs;
    std::vector<u8> stage_inputs;
    std::vector<u8> stage_outputs;
    std::map<u32, usize> uniform_buffers;
    std::map<u32, TextureInfo> textures;

    // Helpers
    void HandleAMemLoad(const AMem amem);
    void HandleCMemLoad(const CMem cmem);
    void HandleAMemStore(const AMem amem);

  public:
    CONST_REF_GETTER(input_svs, GetInputSVs);
    CONST_REF_GETTER(output_svs, GetOutputSVs);
    CONST_REF_GETTER(stage_inputs, GetStageInputs);
    CONST_REF_GETTER(stage_outputs, GetStageOutputs);
    CONST_REF_GETTER(uniform_buffers, GetUniformBuffers);
    CONST_REF_GETTER(textures, GetTextures);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer
