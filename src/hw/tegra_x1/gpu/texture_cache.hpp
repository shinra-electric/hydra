#pragma once

#include "hw/tegra_x1/gpu/texture_decoder.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU {

namespace Renderer {
class TextureBase;
}

class TextureCache : public CacheBase<TextureCache, Renderer::TextureBase*,
                                      Renderer::TextureDescriptor> {
  public:
    void Destroy() {}

    Renderer::TextureBase*
    Create(const Renderer::TextureDescriptor& descriptor);
    void Update(Renderer::TextureBase* texture);
    u64 Hash(const Renderer::TextureDescriptor& descriptor);

    void DestroyElement(Renderer::TextureBase* texture);

  private:
    TextureDecoder texture_decoder;

    // Buffers
    u8 scratch_buffer[0x4000 * 0x4000 * 0x4]; // TODO: allocate dynamically

    // Helpers
    void DecodeTexture(Renderer::TextureBase* texture);
    // TODO: encode texture
};

} // namespace Hydra::HW::TegraX1::GPU
