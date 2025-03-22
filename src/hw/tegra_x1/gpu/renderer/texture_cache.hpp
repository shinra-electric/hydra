#pragma once

#include "hw/tegra_x1/gpu/renderer/texture_decoder.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU::Renderer {

class TextureBase;

class TextureCache
    : public CacheBase<TextureCache, TextureBase*, TextureDescriptor> {
  public:
    void Destroy() {}

    TextureBase* Create(const TextureDescriptor& descriptor);
    void Update(TextureBase* texture);
    u64 Hash(const TextureDescriptor& descriptor);

    void DestroyElement(TextureBase* texture);

  private:
    TextureDecoder texture_decoder;

    // Buffers
    u8 scratch_buffer[0x4000 * 0x4000 * 0x4]; // TODO: allocate dynamically

    // Helpers
    void DecodeTexture(TextureBase* texture);
    // TODO: encode texture
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
