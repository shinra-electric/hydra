#include "hw/tegra_x1/gpu/shader_decompiler/decompiler.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

void Decompiler::Decompile(const u32* code, Renderer::ShaderType type,
                           std::vector<u8>& out_code) {
    // HACK
    switch (type) {
    case Renderer::ShaderType::Vertex:
        // HACK
        static std::string vertex_shader_source = R"(
            #include <metal_stdlib>
            using namespace metal;

            struct VertexIn {
                float3 position [[attribute(0)]];
                float3 color [[attribute(1)]];
            };

            struct VertexOut {
                float4 position [[position]];
                float3 color [[user(locn0)]];
            };

            vertex VertexOut main_(VertexIn in [[stage_in]]) {
                VertexOut out;
                out.position = float4(in.position, 1.0);
                out.color = in.color;

                return out;
            }
        )";
        out_code.assign(vertex_shader_source.begin(),
                        vertex_shader_source.end());
        break;
    case Renderer::ShaderType::Fragment:
        // HACK
        static std::string fragment_shader_source = R"(
            #include <metal_stdlib>
            using namespace metal;

            struct VertexOut {
                float4 position [[position]];
                float3 color [[user(locn0)]];
            };

            fragment float4 main_(VertexOut in [[stage_in]]) {
                return float4(in.color, 1.0);
            }
        )";
        out_code.assign(fragment_shader_source.begin(),
                        fragment_shader_source.end());
        break;
    default:
        LOG_ERROR(ShaderDecompiler, "Unknown shader type {}", type);
        break;
    }
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
