#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU {

namespace Renderer {
class RenderPassBase;
}

class RenderPassCache
    : public CacheBase<RenderPassCache, Renderer::RenderPassBase*,
                       Renderer::RenderPassDescriptor> {
  public:
    void Destroy() {}

    Renderer::RenderPassBase*
    Create(const Renderer::RenderPassDescriptor& descriptor);
    void Update(Renderer::RenderPassBase* render_pass) {}
    u64 Hash(const Renderer::RenderPassDescriptor& descriptor);

    void DestroyElement(Renderer::RenderPassBase* render_pass);
};

} // namespace Hydra::HW::TegraX1::GPU
