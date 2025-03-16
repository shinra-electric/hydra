#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class ObserverBase {
  public:
    // Operations
    virtual void OpExit() = 0;
    virtual void OpMove(reg_t dst, u32 value) = 0;
    virtual void OpLoad(reg_t dst, reg_t src, u64 imm) = 0;
    virtual void OpStore(reg_t src, reg_t dst, u64 imm) = 0;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
