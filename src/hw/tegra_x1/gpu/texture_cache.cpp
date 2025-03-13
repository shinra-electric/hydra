#include "hw/tegra_x1/gpu/texture_cache.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_base.hpp"

// HACK
template <typename T> T rotl(T v, u64 shift) {
    return (v << shift) | (v >> (32 - shift));
}

namespace Hydra::HW::TegraX1::GPU {

Renderer::TextureBase*
TextureCache::Create(const Renderer::TextureDescriptor& descriptor) {
    auto texture = RENDERER->CreateTexture(descriptor);
    DecodeTexture(texture);

    return texture;
}

void TextureCache::Update(Renderer::TextureBase* texture) {
    // TODO: if data changed
    if (false)
        DecodeTexture(texture);
}

u64 TextureCache::Hash(const Renderer::TextureDescriptor& descriptor) {
    u64 hash = 0;
    hash += descriptor.ptr;
    hash = rotl(hash, 13);
    hash += descriptor.width;
    hash = rotl(hash, 11);
    hash += descriptor.height;

    return hash;
}

void TextureCache::DestroyElement(Renderer::TextureBase* texture) {
    delete texture;
}

void TextureCache::DecodeTexture(Renderer::TextureBase* texture) {
    u8* out_data = scratch_buffer + sizeof(scratch_buffer) / 2;
    texture_decoder.Decode(texture->GetDescriptor(), scratch_buffer, out_data);

    RENDERER->UploadTexture(texture, out_data);
}

} // namespace Hydra::HW::TegraX1::GPU
