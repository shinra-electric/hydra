#include "hw/tegra_x1/gpu/shader_cache.hpp"

#include "hw/tegra_x1/gpu/engines/3d.hpp"
#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/renderer/shader_base.hpp"
#include "hw/tegra_x1/gpu/shader_decompiler/decompiler.hpp"

// HACK
template <typename T> T rotl(T v, u64 shift) {
    return (v << shift) | (v >> (32 - shift));
}

namespace Hydra::HW::TegraX1::GPU {

Renderer::ShaderBase*
ShaderCache::Create(const GuestShaderDescriptor& descriptor) {
    Renderer::ShaderDescriptor host_descriptor{};
    host_descriptor.type = Engines::to_renderer_shader_type(descriptor.stage);

    // Decompile
    Reader code_reader(reinterpret_cast<u8*>(descriptor.code_ptr));
    ShaderDecompiler::Decompiler decompiler;
    decompiler.Decompile(code_reader, host_descriptor.type, descriptor.state,
                         host_descriptor.code,
                         host_descriptor.resource_mapping);

    return RENDERER->CreateShader(host_descriptor);
}

u64 ShaderCache::Hash(const GuestShaderDescriptor& descriptor) {
    u64 hash = 0;
    hash += static_cast<u64>(descriptor.stage);
    hash = rotl(hash, 37);
    hash += descriptor.code_ptr;

    // TODO: vertex attribute states
    // TODO: color target formats

    return hash;
}

void ShaderCache::DestroyElement(Renderer::ShaderBase* shader) {
    delete shader;
}

} // namespace Hydra::HW::TegraX1::GPU
