#include "hw/tegra_x1/gpu/shader_cache.hpp"

#include "hw/tegra_x1/gpu/engines/3d.hpp"
#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/renderer/shader_base.hpp"

// HACK
template <typename T> T rotl(T v, u64 shift) {
    return (v << shift) | (v >> (32 - shift));
}

namespace Hydra::HW::TegraX1::GPU {

Renderer::ShaderBase*
ShaderCache::Create(const GuestShaderDescriptor& descriptor) {
    // TODO: guest and host shaders shouldn't have a 1 to 1 mapping

    Renderer::ShaderDescriptor host_descriptor;
    host_descriptor.type = Engines::to_renderer_shader_type(descriptor.stage);
    std::string shader_source;
    switch (host_descriptor.type) {
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
        shader_source = vertex_shader_source;
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
        shader_source = fragment_shader_source;
        break;
    default:
        LOG_ERROR(GPU, "Unknown shader type {}", host_descriptor.type);
        break;
    }

    // TODO: decompile the shader instead
    host_descriptor.code.assign(shader_source.begin(), shader_source.end());

    return RENDERER->CreateShader(host_descriptor);
}

u64 ShaderCache::Hash(const GuestShaderDescriptor& descriptor) {
    u64 hash = 0;
    hash += static_cast<u64>(descriptor.stage);
    hash = rotl(hash, 37);
    hash += descriptor.code_ptr;

    return hash;
}

void ShaderCache::DestroyElement(Renderer::ShaderBase* shader) {
    delete shader;
}

} // namespace Hydra::HW::TegraX1::GPU
