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
    const std::vector<SV>& GetInputSVs() const { return input_svs; }
    const std::vector<SV>& GetOutputSVs() const { return output_svs; }

  private:
    std::vector<SV> input_svs;
    std::vector<SV> output_svs;
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
