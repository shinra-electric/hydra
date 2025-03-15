#pragma once

#include "hw/tegra_x1/gpu/engines/const.hpp"
#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU {

namespace Renderer {
class ShaderBase;
}

struct GuestShaderDescriptor {
    Engines::ShaderStage stage;
    uptr code_ptr;
};

class ShaderCache : public CacheBase<ShaderCache, Renderer::ShaderBase*,
                                     GuestShaderDescriptor> {
  public:
    void Destroy() {}

    Renderer::ShaderBase* Create(const GuestShaderDescriptor& descriptor);
    void Update(Renderer::ShaderBase* shader) {}
    u64 Hash(const GuestShaderDescriptor& descriptor);

    void DestroyElement(Renderer::ShaderBase* shader);
};

} // namespace Hydra::HW::TegraX1::GPU
