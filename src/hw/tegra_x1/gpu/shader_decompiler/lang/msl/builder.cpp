#include "hw/tegra_x1/gpu/shader_decompiler/lang/msl/builder.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL {

void Builder::EmitHeader() {
    Write("#include <metal_stdlib>");
    Write("using namespace metal;");
}

void Builder::EmitTypeAliases() {
    Write("using i8 = int8;");
    Write("using i16 = int16;");
    Write("using i32 = int32;");
    Write("using i64 = int64;");
    Write("using u8 = uint8;");
    Write("using u16 = uint16;");
    Write("using u32 = uint32;");
    Write("using u64 = uint64;");
    Write("using f32 = float;");
    Write("using f64 = double;");
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL
