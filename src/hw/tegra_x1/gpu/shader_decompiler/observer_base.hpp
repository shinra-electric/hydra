#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class ObserverBase {
  public:
    // Operations
    virtual void OpExit() = 0;
    virtual void OpMove(reg_t dst, Operand src) = 0;
    virtual void OpLoad(reg_t dst, IndexedMem src) = 0;
    virtual void OpStore(IndexedMem dst, reg_t src) = 0;
    virtual void OpInterpolate(reg_t dst, IndexedMem src) = 0;
    virtual void OpTextureSample(reg_t dst, u32 index, reg_t coords) = 0;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
