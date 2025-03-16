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

void Analyzer::OpLoad(reg_t dst, reg_t src, u64 imm) {
    // TODO: support indexing with src
    ASSERT_DEBUG(src == RZ, ShaderDecompiler,
                 "Indexing not implemented (src: r{})", src);

    push_sv(input_svs, stage_inputs, imm);
}

void Analyzer::OpStore(reg_t src, reg_t dst, u64 imm) {
    // TODO: support indexing with src
    ASSERT_DEBUG(dst == RZ, ShaderDecompiler,
                 "Indexing not implemented (dst: r{})", dst);

    push_sv(output_svs, stage_outputs, imm);
}

void Analyzer::OpInterpolate(reg_t dst, reg_t src, u64 imm) {
    // TODO: support indexing with src
    ASSERT_DEBUG(src == RZ, ShaderDecompiler,
                 "Indexing not implemented (src: r{})", src);

    push_sv(input_svs, stage_inputs, imm);
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
