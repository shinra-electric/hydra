#pragma once

#include "hw/tegra_x1/gpu/texture_decoder.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU {

namespace Renderer {
class TextureBase;
}

class TextureCache {
  public:
    Renderer::TextureBase*
    FindTexture(const Renderer::TextureDescriptor& descriptor);

  private:
    TextureDecoder texture_decoder;

    std::map<u64, Renderer::TextureBase*> textures;

    // Buffers
    u8 scratch_buffer[0x4000 * 0x4000 * 0x4]; // TODO: allocate dynamically

    u64 CalculateTextureHash(const Renderer::TextureDescriptor& descriptor);
};

} // namespace Hydra::HW::TegraX1::GPU
