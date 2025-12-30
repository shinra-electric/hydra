#include "core/hw/tegra_x1/gpu/renderer/buffer_cache.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/buffer_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {

BufferBase* BufferCache::Create(const BufferDescriptor& descriptor) {
    auto texture = RENDERER_INSTANCE.CreateBuffer(descriptor);
    // TODO: upload buffer

    return texture;
}

void BufferCache::Update(BufferBase* buffer) {
    (void)buffer;

    // TODO: if data changed
    if (false)
        ; // TODO: upload buffer
}

u32 BufferCache::Hash(const BufferDescriptor& descriptor) {
    HashCode hash;
    hash.Add(descriptor.ptr);
    hash.Add(descriptor.size);
    return hash.ToHashCode();
}

void BufferCache::DestroyElement(BufferBase* buffer) { delete buffer; }

} // namespace hydra::hw::tegra_x1::gpu::renderer
