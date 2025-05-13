#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/observer_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::Analyzer {

class MemoryAnalyzer : public ObserverBase {
  public:
    // Operations
    void OpMove(reg_t dst, Operand src) override;
    void OpAdd(Operand dst, Operand src1, Operand src2) override;
    void OpMultiply(Operand dst, Operand src1, Operand src2) override;
    void OpFloatFma(reg_t dst, reg_t src1, Operand src2, Operand src3) override;

    void OpSetPred(ComparisonOperator cmp, BinaryOperator combine_bin,
                   pred_t dst, pred_t combine, Operand lhs,
                   Operand rhs) override;

    void OpLoad(reg_t dst, Operand src) override;
    void OpStore(AMem dst, reg_t src) override;
    void OpInterpolate(reg_t dst, AMem src) override;
    void OpTextureSample(reg_t dst0, reg_t dst1, u32 const_buffer_index,
                         reg_t coords_x, reg_t coords_y) override;

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

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::Analyzer
