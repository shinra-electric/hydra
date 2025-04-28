#pragma once

#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {

class TextureDecoder {
  public:
    TextureDecoder();
    ~TextureDecoder();

    void Decode(const TextureDescriptor& descriptor, u8*& out_data);

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer
