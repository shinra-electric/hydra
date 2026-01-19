#pragma once

#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace hydra::hw::tegra_x1::cpu {
class IMmu;
}

namespace hydra::hw::tegra_x1::gpu::renderer {

class BufferBase;

struct BufferEntry {
    BufferBase* buffer{nullptr};
    std::optional<range<uptr>> invalidation_range{};
};

class BufferCache {
  public:
    ~BufferCache();

    BufferBase* Get(range<uptr> range);

    void InvalidateMemory(range<uptr> range);

  private:
    std::map<uptr, BufferEntry> entries;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer
