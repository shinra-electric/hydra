#include "hw/tegra_x1/gpu/shader_decompiler/analyzer.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

namespace {

template <typename T> void push_unique(std::vector<T>& vec, T value) {
    auto it = std::find_if(vec.begin(), vec.end(),
                           [&](const T v) { return v == value; });
    if (it == vec.end())
        vec.push_back(value);
}

void push_sv(std::vector<SVSemantic>& svs, std::vector<u8>& stage_in_outs,
             u64 addr) {
    const auto sv = GetSVFromAddr(addr);
    if (sv.semantic == SVSemantic::UserInOut)
        push_unique(stage_in_outs, sv.index);
    else
        push_unique(svs, sv.semantic);
}

} // namespace

void Analyzer::OpFloatMultiply(reg_t dst, reg_t src1, Operand src2) {
    if (src2.type == OperandType::ConstMemory) {
        const auto cmem = src2.cmem;
        auto& size = uniform_buffers[cmem.idx];
        size = std::max(size, static_cast<usize>(cmem.imm) + sizeof(u32));
    }
}

void Analyzer::OpLoad(reg_t dst, AMem src) {
    // TODO: support indexing with src
    ASSERT_DEBUG(src.reg == RZ, ShaderDecompiler,
                 "Indexing not implemented (src: r{})", src.reg);

    push_sv(input_svs, stage_inputs, src.imm);
}

void Analyzer::OpStore(AMem dst, reg_t src) {
    // TODO: support indexing with src
    ASSERT_DEBUG(dst.reg == RZ, ShaderDecompiler,
                 "Indexing not implemented (dst: r{})", dst.reg);

    push_sv(output_svs, stage_outputs, dst.imm);
}

void Analyzer::OpInterpolate(reg_t dst, AMem src) {
    // TODO: support indexing with src
    ASSERT_DEBUG(src.reg == RZ, ShaderDecompiler,
                 "Indexing not implemented (src: r{})", src.reg);

    push_sv(input_svs, stage_inputs, src.imm);
}

void Analyzer::OpTextureSample(reg_t dst, u32 index, reg_t coords) {
    push_unique(textures, index);
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
