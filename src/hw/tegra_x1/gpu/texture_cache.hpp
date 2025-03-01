#pragma once

#include "hw/tegra_x1/gpu/const.hpp"
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
    Renderer::TextureBase* FindTexture(const TextureDescriptor& descriptor);

  private:
    TextureDecoder texture_decoder;

    std::map<u64, Renderer::TextureBase*> textures;

    u64 CalculateTextureHash(const TextureDescriptor& descriptor);
};

} // namespace Hydra::HW::TegraX1::GPU
