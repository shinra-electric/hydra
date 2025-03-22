#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU::Renderer {

class BufferBase;

class BufferCache
    : public CacheBase<BufferCache, BufferBase*, BufferDescriptor> {
  public:
    void Destroy() {}

    BufferBase* Create(const BufferDescriptor& descriptor);
    void Update(BufferBase* buffer);
    u64 Hash(const BufferDescriptor& descriptor);

    void DestroyElement(BufferBase* buffer);

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
