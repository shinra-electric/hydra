#pragma once

#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace hydra::hw::tegra_x1::cpu {
class IMmu;
}

namespace hydra::hw::tegra_x1::gpu::renderer {

class BufferBase;

struct BufferEntry {
    BufferBase* buffer{nullptr};
    Range<uptr> range;
    std::optional<Range<uptr>> invalidation_range{};
    bool inline_copy{true}; // TODO: reset to true when not in use
};

class BufferCache {
  public:
    ~BufferCache();

    BufferBase* Get(Range<uptr> range);

    void InvalidateMemory(Range<uptr> range);

  private:
    std::map<uptr, BufferEntry> entries;

    // Helpers
    static void UpdateRange(BufferEntry& entry, Range<uptr> range);
};

} // namespace hydra::hw::tegra_x1::gpu::renderer
