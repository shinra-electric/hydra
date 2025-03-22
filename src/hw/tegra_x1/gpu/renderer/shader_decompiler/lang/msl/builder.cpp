#include "hw/tegra_x1/gpu/renderer/shader_decompiler/lang/msl/builder.hpp"

#include "hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Lang::MSL {

void Builder::InitializeResourceMapping() {
    for (const auto& [index, size] : analyzer.GetUniformBuffers()) {
        out_resource_mapping.uniform_buffers[index] = index;
    }
    // TODO: storage buffers
    for (const auto index : analyzer.GetTextures()) {
        out_resource_mapping.textures[index] = index;
        out_resource_mapping.samplers[index] = index;
    }
    // TODO: images
}

void Builder::EmitHeader() {
    Write("#include <metal_stdlib>");
    Write("using namespace metal;");
}

void Builder::EmitTypeAliases() {
    // Do nothing
}

std::string Builder::GetSVQualifierName(const SV sv, bool output) {
    switch (sv.semantic) {
    case SVSemantic::Position:
        return "[[position]]";
    case SVSemantic::UserInOut:
        switch (type) {
        case ShaderType::Vertex:
            if (output)
                return fmt::format("[[user(locn{})]]", sv.index);
            else
                return fmt::format("[[attribute({})]]", sv.index);
        case ShaderType::Fragment:
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
    case ShaderType::Vertex:
        return "vertex";
    case ShaderType::Fragment:
        return "fragment";
    default:
        return INVALID_VALUE;
    }
}

std::string Builder::EmitTextureSample(u32 index, const std::string& coords) {
    return fmt::format("tex{}.sample(samplr{}, {})", index, index, coords);
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Lang::MSL
