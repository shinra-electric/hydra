#include "hw/tegra_x1/gpu/shader_decompiler/lang/msl/builder.hpp"
#include "hw/tegra_x1/gpu/shader_decompiler/lang/lang_builder_base.hpp"

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

std::string Builder::GetQualifierName(const Qualifier qualifier) {
    switch (qualifier.type) {
    case QualifierType::Position:
        return "[[position]]";
    case QualifierType::UserInOut:
        return fmt::format("[[user(locn{})]]", qualifier.index);
    case QualifierType::StageIn:
        return "[[stage_in]]";
    default:
        LOG_ERROR(ShaderDecompiler, "Unknown qualifier {}", qualifier.type);
        return INVALID_VALUE;
    }
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL
