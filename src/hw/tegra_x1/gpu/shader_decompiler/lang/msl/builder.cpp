#include "hw/tegra_x1/gpu/shader_decompiler/lang/msl/builder.hpp"
#include "hw/tegra_x1/gpu/shader_decompiler/lang/lang_builder_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL {

void Builder::EmitHeader() {
    Write("#include <metal_stdlib>");
    Write("using namespace metal;");
}

void Builder::EmitTypeAliases() {
    Write("using i8 = int8_t;");
    Write("using i16 = int16_t;");
    Write("using i32 = int32_t;");
    Write("using i64 = int64_t;");
    Write("using u8 = uint8_t;");
    Write("using u16 = uint16_t;");
    Write("using u32 = uint32_t;");
    Write("using u64 = uint64_t;");
    Write("using f32 = float;");
    // Write("using f64 = double;");
}

std::string Builder::GetSVQualifierName(const SV sv, bool output) {
    switch (sv.semantic) {
    case SVSemantic::Position:
        return "[[position]]";
    case SVSemantic::UserInOut:
        switch (type) {
        case Renderer::ShaderType::Vertex:
            if (output)
                return fmt::format("[[user(locn{})]]", sv.index);
            else
                return fmt::format("[[attribute({})]]", sv.index);
        case Renderer::ShaderType::Fragment:
            if (output)
                return fmt::format("[[color({})]]", sv.index);
            else
                return fmt::format("[[user(locn{})]]", sv.index);
        default:
            return INVALID_VALUE;
        }
    default:
        LOG_ERROR(ShaderDecompiler, "Unknown SV semantic {}", sv.semantic);
        return INVALID_VALUE;
    }
}

std::string Builder::GetStageQualifierName() {
    switch (type) {
    case Renderer::ShaderType::Vertex:
        return "vertex";
    case Renderer::ShaderType::Fragment:
        return "fragment";
    default:
        return INVALID_VALUE;
    }
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler::Lang::MSL
