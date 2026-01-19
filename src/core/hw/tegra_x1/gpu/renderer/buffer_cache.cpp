#include "core/hw/tegra_x1/gpu/renderer/buffer_cache.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/buffer_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {

BufferCache::~BufferCache() {
    for (auto& entry : entries)
        delete entry.second.buffer;
}

BufferBase* BufferCache::Get(Range<uptr> range) {
    auto& entry = entries[range.GetBegin()];
    if (entry.buffer) {
        if (entry.buffer->GetSize() <
            range.GetSize()) { // Check if the size is sufficient
            // TODO: is this safe?
            delete entry.buffer;
            entry.buffer = RENDERER_INSTANCE.CreateBuffer(range.GetSize());
            entry.range = range;
            UpdateRange(entry, range);
        } else if (entry.invalidation_range
                       .has_value()) { // Check for memory invalidation
            const auto invalidation_range = entry.invalidation_range.value();
            UpdateRange(entry, invalidation_range);
            entry.invalidation_range = std::nullopt;
        }
    } else {
        // Create new buffer
        entry.buffer = RENDERER_INSTANCE.CreateBuffer(range.GetSize());
        entry.range = range;
        UpdateRange(entry, range);
    }

    return entry.buffer;
}

// TODO: make this more efficient
void BufferCache::InvalidateMemory(Range<uptr> range) {
    for (auto& [ptr, entry] : entries) {
        const auto crnt_range =
            Range<uptr>::FromSize(ptr, entry.buffer->GetSize());
        if (crnt_range.Intersects(range)) {
            // Clamp the range
            auto invalidation_range = range.ClampedTo(crnt_range);

            // Combine with an existing invalidation range if it exists
            if (entry.invalidation_range.has_value()) {
                invalidation_range =
                    invalidation_range.Union(entry.invalidation_range.value());
            }

            // Update the invalidation range
            entry.invalidation_range = invalidation_range;
        }
    }
}

void BufferCache::UpdateRange(BufferEntry& entry, Range<uptr> range) {
    if (entry.inline_copy) {
        // Do an inline update if possible
        entry.buffer->CopyFrom(range.GetBegin(),
                               range.GetBegin() - entry.range.GetBegin(),
                               range.GetSize());
        entry.inline_copy = false;
    } else {
        // Copy from a temporary buffer
        auto tmp_buffer =
            RENDERER_INSTANCE.AllocateTemporaryBuffer(range.GetSize());
        tmp_buffer->CopyFrom(range.GetBegin());
        entry.buffer->CopyFrom(tmp_buffer,
                               range.GetBegin() - entry.range.GetBegin(), 0,
                               range.GetSize());
        RENDERER_INSTANCE.FreeTemporaryBuffer(tmp_buffer);
    }
}

} // namespace hydra::hw::tegra_x1::gpu::renderer
