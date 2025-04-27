#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

namespace {

void push_sv(std::vector<SVSemantic>& svs, std::vector<u8>& stage_in_outs,
             u64 addr) {
    const auto sv = get_sv_from_addr(addr);
    if (sv.semantic == SVSemantic::UserInOut)
        push_unique(stage_in_outs, sv.index);
    else
        push_unique(svs, sv.semantic);
}

} // namespace

void Analyzer::OpAdd(Operand dst, Operand src1, Operand src2) {
    HandleLoad(src2);
}

void Analyzer::OpMultiply(Operand dst, Operand src1, Operand src2) {
    HandleLoad(src2);
}

void Analyzer::OpFloatFma(reg_t dst, reg_t src1, Operand src2, Operand src3) {
    HandleLoad(src2);
    HandleLoad(src3);
}

void Analyzer::OpLoad(reg_t dst, Operand src) { HandleLoad(src); }

void Analyzer::OpStore(AMem dst, reg_t src) { HandleAMemStore(dst); }

void Analyzer::OpInterpolate(reg_t dst, AMem src) { HandleAMemLoad(src); }

void Analyzer::OpTextureSample(reg_t dst0, reg_t dst1, u32 const_buffer_index,
                               reg_t coords_x, reg_t coords_y) {
    push_unique(textures, const_buffer_index);
}

void Analyzer::HandleAMemLoad(const AMem amem) {
    // TODO: support indexing with src
    ASSERT_DEBUG(amem.reg == RZ, ShaderDecompiler,
                 "Indexing not implemented (src: r{})", amem.reg);
    push_sv(input_svs, stage_inputs, amem.imm);
}

void Analyzer::HandleCMemLoad(const CMem cmem) {
    if (cmem.reg != RZ) {
        LOG_WARN(ShaderDecompiler, "Indexing not implemented (src: r{})",
                 cmem.reg);
        return;
    }

    auto& size = uniform_buffers[cmem.idx];
    size = std::max(size, static_cast<usize>(cmem.imm) + sizeof(u32));
}

void Analyzer::HandleLoad(const Operand operand) {
    switch (operand.type) {
    case OperandType::Register:
    case OperandType::Immediate:
        break;
    case OperandType::AttributeMemory:
        HandleAMemLoad(operand.amem);
        break;
    case OperandType::ConstMemory:
        HandleCMemLoad(operand.cmem);
        break;
    default:
        LOG_NOT_IMPLEMENTED(ShaderDecompiler, "Operand type {}", operand.type);
        break;
    }
}

void Analyzer::HandleAMemStore(const AMem amem) {
    // TODO: support indexing with src
    ASSERT_DEBUG(amem.reg == RZ, ShaderDecompiler,
                 "Indexing not implemented (src: r{})", amem.reg);
    push_sv(output_svs, stage_outputs, amem.imm);
}

void Analyzer::HandleStore(const Operand operand) {
    switch (operand.type) {
    case OperandType::Register:
    case OperandType::Immediate:
        break;
    case OperandType::AttributeMemory:
        HandleAMemStore(operand.amem);
        break;
    default:
        LOG_NOT_IMPLEMENTED(ShaderDecompiler, "Operand type {}", operand.type);
        break;
    }
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler
