#include "hw/tegra_x1/gpu/renderer/texture_cache.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

TextureBase* TextureCache::Create(const TextureDescriptor& descriptor) {
    auto texture = RENDERER->CreateTexture(descriptor);
    DecodeTexture(texture);

    return texture;
}

void TextureCache::Update(TextureBase* texture) {
    // TODO: if data changed
    if (true)
        DecodeTexture(texture);
}

u64 TextureCache::Hash(const TextureDescriptor& descriptor) {
    u64 hash = 0;
    hash += descriptor.ptr;
    hash = rotl(hash, 7);
    hash += static_cast<u64>(descriptor.format);
    hash = rotl(hash, 13);
    hash += descriptor.width;
    hash = rotl(hash, 11);
    hash += descriptor.height;

    return hash;
}

void TextureCache::DestroyElement(TextureBase* texture) { delete texture; }

void TextureCache::DecodeTexture(TextureBase* texture) {
    u8* out_data = scratch_buffer + sizeof(scratch_buffer) / 2;
    texture_decoder.Decode(texture->GetDescriptor(), scratch_buffer, out_data);

    texture->CopyFrom(out_data);
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer
