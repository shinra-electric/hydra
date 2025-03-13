#include "hw/tegra_x1/gpu/render_pass_cache.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/renderer/render_pass_base.hpp"

// HACK
template <typename T> T rotl(T v, u64 shift) {
    return (v << shift) | (v >> (32 - shift));
}

namespace Hydra::HW::TegraX1::GPU {

Renderer::RenderPassBase*
RenderPassCache::Create(const Renderer::RenderPassDescriptor& descriptor) {
    auto render_pass = RENDERER->CreateRenderPass(descriptor);

    return render_pass;
}

u64 RenderPassCache::Hash(const Renderer::RenderPassDescriptor& descriptor) {
    // TODO: implement
    return 0;
}

void RenderPassCache::DestroyElement(Renderer::RenderPassBase* render_pass) {
    delete render_pass;
}

} // namespace Hydra::HW::TegraX1::GPU
