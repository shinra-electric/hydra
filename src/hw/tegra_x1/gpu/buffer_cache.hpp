#pragma once

#include "hw/tegra_x1/gpu/texture_decoder.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU {

namespace Renderer {
class BufferBase;
}

class BufferCache : public CacheBase<BufferCache, Renderer::BufferBase*,
                                     Renderer::BufferDescriptor> {
  public:
    void Destroy() {}

    Renderer::BufferBase* Create(const Renderer::BufferDescriptor& descriptor);
    void Update(Renderer::BufferBase* buffer);
    u64 Hash(const Renderer::BufferDescriptor& descriptor);

    void DestroyElement(Renderer::BufferBase* buffer);

  private:
};

} // namespace Hydra::HW::TegraX1::GPU
