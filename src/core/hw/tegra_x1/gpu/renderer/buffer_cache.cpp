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

    // Check if the size is sufficient
    if (entry.buffer && entry.buffer->GetSize() < range.GetSize()) {
        // TODO: is this safe?
        delete entry.buffer;
        entry.buffer = nullptr;
    }

    // Check for new buffers
    if (!entry.buffer) {
        entry.buffer = RENDERER_INSTANCE.CreateBuffer(range.GetSize());

        // Copy from temporary buffer
        auto tmp_buffer =
            RENDERER_INSTANCE.AllocateTemporaryBuffer(range.GetSize());
        tmp_buffer->CopyFrom(range.GetBegin());
        entry.buffer->CopyFrom(tmp_buffer);
        RENDERER_INSTANCE.FreeTemporaryBuffer(tmp_buffer);

        return entry.buffer;
    }

    // Check for memory invalidation
    if (entry.invalidation_range.has_value()) {
        const auto invalidation_range = entry.invalidation_range.value();

        // Copy from temporary buffer
        auto tmp_buffer = RENDERER_INSTANCE.AllocateTemporaryBuffer(
            invalidation_range.GetSize());
        tmp_buffer->CopyFrom(invalidation_range.GetBegin());
        entry.buffer->CopyFrom(tmp_buffer,
                               invalidation_range.GetBegin() - range.GetBegin(),
                               0, invalidation_range.GetSize());
        RENDERER_INSTANCE.FreeTemporaryBuffer(tmp_buffer);

        entry.invalidation_range = std::nullopt;
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

} // namespace hydra::hw::tegra_x1::gpu::renderer
