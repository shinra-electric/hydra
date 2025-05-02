#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer {

class MemoryAnalyzer {
  public:
    // Operations
    void OpAdd(Operand dst, Operand src1, Operand src2);
    void OpMultiply(Operand dst, Operand src1, Operand src2);
    void OpFloatFma(reg_t dst, reg_t src1, Operand src2, Operand src3);
    void OpLoad(reg_t dst, Operand src);
    void OpStore(AMem dst, reg_t src);
    void OpInterpolate(reg_t dst, AMem src);
    void OpTextureSample(reg_t dst0, reg_t dst1, u32 const_buffer_index,
                         reg_t coords_x, reg_t coords_y);

    // Getters
    const std::vector<SvSemantic>& GetInputSVs() const { return input_svs; }
    const std::vector<SvSemantic>& GetOutputSVs() const { return output_svs; }
    const std::vector<u8>& GetStageInputs() const { return stage_inputs; }
    const std::vector<u8>& GetStageOutputs() const { return stage_outputs; }
    const std::map<u32, usize>& GetUniformBuffers() const {
        return uniform_buffers;
    }
    const std::vector<u32>& GetTextures() const { return textures; }

  private:
    std::vector<SvSemantic> input_svs;
    std::vector<SvSemantic> output_svs;
    std::vector<u8> stage_inputs;
    std::vector<u8> stage_outputs;
    std::map<u32, usize> uniform_buffers;
    std::vector<u32> textures;

    // Helpers
    void HandleAMemLoad(const AMem amem);
    void HandleCMemLoad(const CMem cmem);
    void HandleLoad(const Operand operand);

    void HandleAMemStore(const AMem amem);
    void HandleStore(const Operand operand);
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer
