#include "hw/tegra_x1/gpu/shader_decompiler/analyzer.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

namespace {

void push_sv(std::vector<SV>& svs, u64 addr) {
    const auto sv = GetSVFromAddr(addr);
    auto it = std::find_if(svs.begin(), svs.end(), [&](const SV sv_) {
        return (sv.semantic == sv_.semantic && sv.index == sv_.index);
    });
    if (it == svs.end())
        svs.push_back(sv);
}

} // namespace

void Analyzer::OpLoad(reg_t dst, reg_t src, u64 imm) {
    // TODO: support indexing with src
    ASSERT_DEBUG(src == RZ, ShaderDecompiler,
                 "Indexing not implemented (src: r{})", src);

    push_sv(input_svs, imm);
}

void Analyzer::OpStore(reg_t src, reg_t dst, u64 imm) {
    // TODO: support indexing with src
    ASSERT_DEBUG(dst == RZ, ShaderDecompiler,
                 "Indexing not implemented (dst: r{})", dst);

    push_sv(output_svs, imm);
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
