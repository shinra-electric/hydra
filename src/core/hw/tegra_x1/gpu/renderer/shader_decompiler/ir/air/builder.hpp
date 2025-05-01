#pragma once

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Transforms/Scalar/Scalarizer.h"

#include "luft/luft.hpp"

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::IR::AIR {

class Builder final : public BuilderBase {
  public:
    Builder(const Analyzer::Analyzer& analyzer, const ShaderType type,
            const GuestShaderState& state, std::vector<u8>& out_code,
            ResourceMapping& out_resource_mapping);

    void Start() override;
    void Finish() override;

    // Operations
    void OpExit() override;
    void OpMove(reg_t dst, Operand src) override;
    void OpAdd(Operand dst, Operand src1, Operand src2) override;
    void OpMultiply(Operand dst, Operand src1, Operand src2) override;
    void OpFloatFma(reg_t dst, reg_t src1, Operand src2, Operand src3) override;
    void OpShiftLeft(reg_t dst, reg_t src, u32 shift) override;
    void OpMathFunction(MathFunc func, reg_t dst, reg_t src) override;
    void OpLoad(reg_t dst, Operand src) override;
    void OpStore(AMem dst, reg_t src) override;
    void OpInterpolate(reg_t dst, AMem src) override;
    void OpTextureSample(reg_t dst0, reg_t dst1, u32 const_buffer_index,
                         reg_t coords_x, reg_t coords_y) override;

  private:
    llvm::LLVMContext context;
    llvm::Module module;

    luft::AirType types;

    llvm::IRBuilder<>* builder;

    void RunOptimizationPasses(llvm::OptimizationLevel opt);
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::IR::AIR
