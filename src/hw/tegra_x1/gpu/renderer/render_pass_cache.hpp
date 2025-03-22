#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class RenderPassBase;

class RenderPassCache
    : public CacheBase<RenderPassCache, RenderPassBase*, RenderPassDescriptor> {
  public:
    void Destroy() {}

    RenderPassBase* Create(const RenderPassDescriptor& descriptor);
    void Update(RenderPassBase* render_pass) {}
    u64 Hash(const RenderPassDescriptor& descriptor);

    void DestroyElement(RenderPassBase* render_pass);
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
