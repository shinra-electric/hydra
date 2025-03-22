#pragma once

#include "hw/tegra_x1/gpu/renderer/shader_decompiler/observer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

class Analyzer : public ObserverBase {
  public:
    // Operations
    void OpExit() override {}
    void OpMove(reg_t dst, Operand src) override {}
    void OpFloatMultiply(reg_t dst, reg_t src1, Operand src2) override;
    void OpLoad(reg_t dst, AMem src) override;
    void OpStore(AMem dst, reg_t src) override;
    void OpInterpolate(reg_t dst, AMem src) override;
    void OpTextureSample(reg_t dst, u32 index, reg_t coords) override;

    // Getters
    const std::vector<SVSemantic>& GetInputSVs() const { return input_svs; }
    const std::vector<SVSemantic>& GetOutputSVs() const { return output_svs; }
    const std::vector<u8>& GetStageInputs() const { return stage_inputs; }
    const std::vector<u8>& GetStageOutputs() const { return stage_outputs; }
    const std::map<u32, usize>& GetUniformBuffers() const {
        return uniform_buffers;
    }
    const std::vector<u32>& GetTextures() const { return textures; }

  private:
    std::vector<SVSemantic> input_svs;
    std::vector<SVSemantic> output_svs;
    std::vector<u8> stage_inputs;
    std::vector<u8> stage_outputs;
    std::map<u32, usize> uniform_buffers;
    std::vector<u32> textures;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler
