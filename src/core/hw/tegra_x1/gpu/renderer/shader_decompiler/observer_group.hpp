#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/observer_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp {

template <usize N> class ObserverGroup : public ObserverBase {
  public:
    ObserverGroup(const std::array<ObserverBase*, N>& observers_)
        : observers{observers_} {}

#define FOR_EACH_OBSERVER()                                                    \
    for (u32 i = 0; i < N; ++i)                                                \
    observers[i]

    void SetPredCond(const PredCond pred_cond) override {
        FOR_EACH_OBSERVER()->SetPredCond(pred_cond);
    }
    void ClearPredCond() override { FOR_EACH_OBSERVER()->ClearPredCond(); }
    void BlockChanged() override { FOR_EACH_OBSERVER()->BlockChanged(); }

    // Operations

    // Basic
    void OpMove(reg_t dst, Operand src) override {
        FOR_EACH_OBSERVER()->OpMove(dst, src);
    }
    void OpAdd(Operand dst, Operand src1, Operand src2) override {
        FOR_EACH_OBSERVER()->OpAdd(dst, src1, src2);
    }
    void OpMultiply(Operand dst, Operand src1, Operand src2) override {
        FOR_EACH_OBSERVER()->OpMultiply(dst, src1, src2);
    }
    void OpFloatFma(reg_t dst, reg_t src1, Operand src2,
                    Operand src3) override {
        FOR_EACH_OBSERVER()->OpFloatFma(dst, src1, src2, src3);
    }
    void OpShiftLeft(reg_t dst, reg_t src, u32 shift) override {
        FOR_EACH_OBSERVER()->OpShiftLeft(dst, src, shift);
    }
    void OpCast(Operand dst, Operand src) override {
        FOR_EACH_OBSERVER()->OpCast(dst, src);
    }

    // Control flow
    void OpSetPred(ComparisonOperator cmp, BinaryOperator combine_bin,
                   pred_t dst, pred_t combine, Operand lhs,
                   Operand rhs) override {
        FOR_EACH_OBSERVER()->OpSetPred(cmp, combine_bin, dst, combine, lhs,
                                       rhs);
    }
    void OpSetSync(u32 target) override {
        FOR_EACH_OBSERVER()->OpSetSync(target);
    }
    void OpSync() override { FOR_EACH_OBSERVER()->OpSync(); }
    void OpBranch(u32 target) override {
        FOR_EACH_OBSERVER()->OpBranch(target);
    }
    void OpExit() override { FOR_EACH_OBSERVER()->OpExit(); }

    // Special
    void OpMathFunction(MathFunc func, reg_t dst, reg_t src) override {
        FOR_EACH_OBSERVER()->OpMathFunction(func, dst, src);
    }
    void OpLoad(reg_t dst, Operand src) override {
        FOR_EACH_OBSERVER()->OpLoad(dst, src);
    }
    void OpStore(AMem dst, reg_t src) override {
        FOR_EACH_OBSERVER()->OpStore(dst, src);
    }
    void OpInterpolate(reg_t dst, AMem src) override {
        FOR_EACH_OBSERVER()->OpInterpolate(dst, src);
    }
    void OpTextureSample(reg_t dst0, reg_t dst1, u32 const_buffer_index,
                         reg_t coords_x, reg_t coords_y) override {
        FOR_EACH_OBSERVER()->OpTextureSample(dst0, dst1, const_buffer_index,
                                             coords_x, coords_y);
    }
    void OpDiscard() override { FOR_EACH_OBSERVER()->OpDiscard(); }

    // Setters
    void SetPC(const u32 pc) override { FOR_EACH_OBSERVER()->SetPC(pc); }

#undef FOR_EACH_OBSERVER

  protected:
    std::array<ObserverBase*, N> observers;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp
