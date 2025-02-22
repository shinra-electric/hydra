#include "hw/tegra_x1/gpu/texture_cache.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"

// HACK
template <typename T> T rotl(T v, u64 shift) {
    return (v << shift) | (v >> (32 - shift));
}

namespace Hydra::HW::TegraX1::GPU {

Renderer::TextureBase*
TextureCache::FindTexture(const TextureDescriptor& descriptor) {
    auto render = GPU::GetInstance().GetRenderer();

    u64 hash = CalculateTextureHash(descriptor);
    auto& texture = textures[hash];

    bool dirty = false;
    if (!texture) {
        texture = render->CreateTexture(descriptor);
        dirty = true;
    } else {
        // TODO: if data changed
        if (true)
            dirty = true;
    }

    if (dirty) {
        // TODO: decode
        uptr data = descriptor.ptr;

        render->UploadTexture(texture, reinterpret_cast<void*>(data));
    }

    return texture;
}

u64 TextureCache::CalculateTextureHash(const TextureDescriptor& descriptor) {
    u64 hash = 0;
    hash += descriptor.ptr;
    hash = rotl(hash, 13);
    hash += descriptor.width;
    hash = rotl(hash, 11);
    hash += descriptor.height;

    return hash;
}

} // namespace Hydra::HW::TegraX1::GPU
