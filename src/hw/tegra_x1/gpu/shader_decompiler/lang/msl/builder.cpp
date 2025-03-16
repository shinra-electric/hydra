#include "hw/tegra_x1/gpu/shader_decompiler/lang/msl/builder.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL {

void Builder::EmitHeader() {
    Write("#include <metal_stdlib>");
    Write("using namespace metal;");
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL
