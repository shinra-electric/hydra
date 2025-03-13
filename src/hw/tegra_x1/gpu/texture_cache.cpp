#include "hw/tegra_x1/gpu/texture_cache.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"

// HACK
template <typename T> T rotl(T v, u64 shift) {
    return (v << shift) | (v >> (32 - shift));
}

namespace Hydra::HW::TegraX1::GPU {

Renderer::TextureBase*
TextureCache::FindTexture(const Renderer::TextureDescriptor& descriptor) {
    auto render = GPU::GetInstance().GetRenderer();

    u64 hash = CalculateTextureHash(descriptor);
    auto& texture = textures[hash];

    bool cpu_dirty = false;
    if (!texture) {
        texture = render->CreateTexture(descriptor);
        cpu_dirty = true;
    } else {
        // TODO: if data changed
        if (false)
            cpu_dirty = true;
    }

    if (cpu_dirty) {
        u8* out_data = scratch_buffer + sizeof(scratch_buffer) / 2;
        texture_decoder.Decode(descriptor, scratch_buffer, out_data);

        render->UploadTexture(texture, out_data);
    }

    return texture;
}

u64 TextureCache::CalculateTextureHash(
    const Renderer::TextureDescriptor& descriptor) {
    u64 hash = 0;
    hash += descriptor.ptr;
    hash = rotl(hash, 13);
    hash += descriptor.width;
    hash = rotl(hash, 11);
    hash += descriptor.height;

    return hash;
}

} // namespace Hydra::HW::TegraX1::GPU
