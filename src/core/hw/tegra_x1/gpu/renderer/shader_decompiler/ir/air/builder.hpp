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
#include <bit>

#include "luft/luft.hpp"

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::IR::AIR {

class Builder final : public BuilderBase {
  public:
    Builder(const Analyzer::Analyzer& analyzer, const ShaderType type,
            const GuestShaderState& state, std::vector<u8>& out_code,
            ResourceMapping& out_resource_mapping);

    void InitializeResourceMapping() override;
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

    // Types
    llvm::Type* regs_ty;
    llvm::Type* amem_ty;
    llvm::Type* cmem_ty;

    // Allocas
    llvm::AllocaInst* regs_v;
    llvm::AllocaInst* amem_v;
    llvm::AllocaInst* cmem_v;

    void InitializeSignature(luft::FunctionSignatureBuilder& signature_builder);
    void RunOptimizationPasses(llvm::OptimizationLevel opt);

    // Helpers
    llvm::Type* GetLlvmType(DataType data_type) {
        switch (data_type) {
        case DataType::Int:
        case DataType::UInt:
            return types._int;
        case DataType::Float:
            return types._float;
        default:
            LOG_ERROR(ShaderDecompiler, "Unsupported data type");
            return nullptr;
        }
    }

    llvm::Value* GetReg(reg_t reg, bool write = false,
                        DataType data_type = DataType::UInt) {
        if (reg == RZ && !write)
            return GetImmediate(0, data_type);

        auto res_v = builder->CreateGEP(regs_ty, regs_v, {GetImmediate(reg)});
        if (!write)
            return builder->CreateLoad(GetLlvmType(data_type), res_v);

        return res_v;
    }

    llvm::Constant* GetImmediate(u32 imm, DataType data_type = DataType::UInt) {
        switch (data_type) {
        // TODO: same for int and uint?
        case DataType::Int:
        case DataType::UInt:
            return llvm::ConstantInt::get(context, llvm::APInt(32, imm));
        case DataType::Float:
            return llvm::ConstantFP::get(
                context, llvm::APFloat(std::bit_cast<f32>(imm)));
        default:
            LOG_ERROR(ShaderDecompiler, "Unsupported data type");
            return nullptr;
        }
    }

    llvm::Value* GetA(const AMem amem, bool write = false,
                      DataType data_type = DataType::UInt) {
        // TODO: support indexing with reg
        auto res_v = builder->CreateGEP(amem_ty, amem_v,
                                        {GetImmediate(amem.imm / sizeof(u32))});
        if (!write)
            return builder->CreateLoad(GetLlvmType(data_type), res_v);

        return res_v;
    }

    llvm::Value* GetC(const CMem cmem, bool write = false,
                      DataType data_type = DataType::UInt) {
        auto res_v = builder->CreateGEP(
            cmem_ty, cmem_v,
            {GetReg(cmem.reg), GetImmediate(cmem.imm / sizeof(u32))});
        if (!write)
            return builder->CreateLoad(GetLlvmType(data_type), res_v);

        return res_v;
    }

    llvm::Value* GetOperand(Operand operand, bool write = false) {
        llvm::Value* res;
        switch (operand.type) {
        case OperandType::Register:
            res = GetReg(operand.reg, write, operand.data_type);
            break;
        case OperandType::Immediate:
            res = GetImmediate(operand.imm, operand.data_type);
            break;
        case OperandType::AttributeMemory:
            res = GetA(operand.amem, write, operand.data_type);
            break;
        case OperandType::ConstMemory:
            res = GetC(operand.cmem, write, operand.data_type);
            break;
        }

        if (operand.neg)
            return builder->CreateNeg(res);
        else
            return res;
    }
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::IR::AIR
