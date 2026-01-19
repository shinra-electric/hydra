#pragma once

#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace hydra::hw::tegra_x1::cpu {
class IMmu;
}

namespace hydra::hw::tegra_x1::gpu::renderer {

class BufferBase;

struct BufferEntry {
    BufferBase* buffer{nullptr};
    std::optional<Range<uptr>> invalidation_range{};
};

class BufferCache {
  public:
    ~BufferCache();

    BufferBase* Get(Range<uptr> range);

    void InvalidateMemory(Range<uptr> range);

  private:
    std::map<uptr, BufferEntry> entries;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer
