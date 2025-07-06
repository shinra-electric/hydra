#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/function.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {
class GuestShaderState;
}

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::analyzer {
class MemoryAnalyzer;
}

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::codegen {

class IEmitter {
  public:
    IEmitter(const DecompilerContext& context_,
             const analyzer::MemoryAnalyzer& memory_analyzer_,
             const GuestShaderState& state_, std::vector<u8>& out_code_,
             ResourceMapping& out_resource_mapping_)
        : context{context_}, memory_analyzer{memory_analyzer_}, state{state_},
          out_code{out_code_}, out_resource_mapping{out_resource_mapping_} {}
    virtual ~IEmitter() {}

    virtual void InitializeResourceMapping() = 0;
    virtual void BeginFunction(const std::string_view name) = 0;
    virtual void EndFunction() = 0;

    // Emit

    // Basic
    virtual void EmitCopy(const ir::Value& dst, const ir::Value& src) = 0;
    virtual void OpAdd(const ir::Value& dst, const ir::Value& srcA,
                       const ir::Value& srcB) = 0;
    virtual void OpMultiply(const ir::Value& dst, const ir::Value& srcA,
                            const ir::Value& srcB) = 0;
    virtual void OpFma(const ir::Value& dst, const ir::Value& srcA,
                       const ir::Value& srcB, const ir::Value& srcC) = 0;
    virtual void OpShiftLeft(const ir::Value& dst, const ir::Value& src,
                             u32 shift) = 0;
    virtual void OpShiftRight(const ir::Value& dst, const ir::Value& src,
                              u32 shift) = 0;
    virtual void OpCast(const ir::Value& dst, const ir::Value& src,
                        DataType dst_type) = 0;
    virtual void OpCompare(const ir::Value& dst, ComparisonOp cmp,
                           const ir::Value& srcA, const ir::Value& srcB) = 0;
    virtual void OpBitwise(const ir::Value& dst, BitwiseOp bin,
                           const ir::Value& srcA, const ir::Value& srcB) = 0;
    virtual void OpSelect(const ir::Value& dst, const ir::Value& cond,
                          const ir::Value& src_true,
                          const ir::Value& src_false) = 0;

    // Control flow
    virtual void OpBranch(label_t target) = 0;
    virtual void OpBranchConditional(const ir::Value& cond, label_t target_true,
                                     label_t target_false) = 0;
    virtual void OpBeginIf(const ir::Value& cond) = 0;
    virtual void OpEndIf() = 0;

    // Math
    virtual void OpMin(const ir::Value& dst, const ir::Value& srcA,
                       const ir::Value& srcB) = 0;
    virtual void OpMax(const ir::Value& dst, const ir::Value& srcA,
                       const ir::Value& srcB) = 0;
    virtual void OpMathFunction(const ir::Value& dst, MathFunc func,
                                const ir::Value& src) = 0;

    // Vector
    virtual void OpVectorConstruct(const ir::Value& dst, DataType data_type,
                                   const std::vector<ir::Value>& elements) = 0;

    // Special
    virtual void OpExit() = 0;
    virtual void OpDiscard() = 0;
    virtual void OpTextureSample(const ir::Value& dst, u32 const_buffer_index,
                                 const ir::Value& coords) = 0;
    virtual void OpTextureRead(const ir::Value& dst, u32 const_buffer_index,
                               const ir::Value& coords) = 0;

  protected:
    const DecompilerContext& context;
    const analyzer::MemoryAnalyzer& memory_analyzer;
    const GuestShaderState& state;

    std::vector<u8>& out_code;
    ResourceMapping& out_resource_mapping;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::codegen
