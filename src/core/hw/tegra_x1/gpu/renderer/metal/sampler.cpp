#include "core/hw/tegra_x1/gpu/renderer/metal/sampler.hpp"

#include "core/hw/tegra_x1/gpu/renderer/metal/maxwell_to_mtl.hpp"
#include "core/hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::metal {

Sampler::Sampler(const SamplerDescriptor& descriptor)
    : SamplerBase(descriptor) {
    MTL::SamplerDescriptor* desc = MTL::SamplerDescriptor::alloc()->init();
    // TODO

    mtl_sampler = METAL_RENDERER_INSTANCE.GetDevice()->newSamplerState(desc);
}

Sampler::~Sampler() { mtl_sampler->release(); }

} // namespace hydra::hw::tegra_x1::gpu::renderer::metal
