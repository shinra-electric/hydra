#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/observer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class Analyzer : public ObserverBase {
  public:
    // Operations
    void OpExit() override {}
    void OpMove(reg_t dst, u32 value) override {}
    void OpLoad(reg_t dst, reg_t src, u64 imm) override;
    void OpStore(reg_t src, reg_t dst, u64 imm) override;

    // Getters
    const std::vector<SVSemantic>& GetInputSVs() const { return input_svs; }
    const std::vector<SVSemantic>& GetOutputSVs() const { return output_svs; }
    const std::vector<u8>& GetStageInputs() const { return stage_inputs; }
    const std::vector<u8>& GetStageOutputs() const { return stage_outputs; }

  private:
    std::vector<SVSemantic> input_svs;
    std::vector<SVSemantic> output_svs;
    std::vector<u8> stage_inputs;
    std::vector<u8> stage_outputs;
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
