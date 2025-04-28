#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/memory_analyzer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer {

class Analyzer : public ObserverBase {
  public:
    // Operations
    void OpExit() override { mem_analyzer.OpExit(); }

    void OpMove(reg_t dst, Operand src) override {
        mem_analyzer.OpMove(dst, src);
    }

    void OpAdd(Operand dst, Operand src1, Operand src2) override {
        mem_analyzer.OpAdd(dst, src1, src2);
    }

    void OpMultiply(Operand dst, Operand src1, Operand src2) override {
        mem_analyzer.OpMultiply(dst, src1, src2);
    }

    void OpFloatFma(reg_t dst, reg_t src1, Operand src2,
                    Operand src3) override {
        mem_analyzer.OpFloatFma(dst, src1, src2, src3);
    }

    void OpShiftLeft(reg_t dst, reg_t src, u32 shift) override {
        mem_analyzer.OpShiftLeft(dst, src, shift);
    }

    void OpMathFunction(MathFunc func, reg_t dst, reg_t src) override {
        mem_analyzer.OpMathFunction(func, dst, src);
    }

    void OpLoad(reg_t dst, Operand src) override {
        mem_analyzer.OpLoad(dst, src);
    }

    void OpStore(AMem dst, reg_t src) override {
        mem_analyzer.OpStore(dst, src);
    }

    void OpInterpolate(reg_t dst, AMem src) override {
        mem_analyzer.OpInterpolate(dst, src);
    }

    void OpTextureSample(reg_t dst0, reg_t dst1, u32 const_buffer_index,
                         reg_t coords_x, reg_t coords_y) override {
        mem_analyzer.OpTextureSample(dst0, dst1, const_buffer_index, coords_x,
                                     coords_y);
    }

    // Getters
    const MemoryAnalyzer& GetMemoryAnalyzer() const { return mem_analyzer; }

  private:
    MemoryAnalyzer mem_analyzer;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Analyzer
