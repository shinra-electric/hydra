#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

class ObserverBase {
  public:
    // Operations
    virtual void OpExit() {}
    virtual void OpMove(reg_t dst, Operand src) {}
    virtual void OpAdd(Operand dst, Operand src1, Operand src2) {}
    virtual void OpMultiply(Operand dst, Operand src1, Operand src2) {}
    virtual void OpFloatFma(reg_t dst, reg_t src1, Operand src2, Operand src3) {
    }
    virtual void OpShiftLeft(reg_t dst, reg_t src, u32 shift) {}
    virtual void OpMathFunction(MathFunc func, reg_t dst, reg_t src) {}
    virtual void OpLoad(reg_t dst, Operand src) {}
    virtual void OpStore(AMem dst, reg_t src) {}
    virtual void OpInterpolate(reg_t dst, AMem src) {}
    virtual void OpTextureSample(reg_t dst0, reg_t dst1, u32 const_buffer_index,
                                 reg_t coords_x, reg_t coords_y) {}

    // Setters
    void SetPC(const u32 pc_) { pc = pc_; }

  protected:
    u32 pc{0};
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler
