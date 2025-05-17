#include "core/hw/tegra_x1/gpu/renderer/sampler_cache.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/sampler_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {

SamplerBase* SamplerCache::Create(const SamplerDescriptor& descriptor) {
    auto sampler = RENDERER_INSTANCE->CreateSampler(descriptor);
    return sampler;
}

u64 SamplerCache::Hash(const SamplerDescriptor& descriptor) {
    u64 hash = 0;
    // TODO

    return hash;
}

void SamplerCache::DestroyElement(SamplerBase* sampler) { delete sampler; }

} // namespace hydra::hw::tegra_x1::gpu::renderer
