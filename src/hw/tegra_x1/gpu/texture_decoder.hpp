#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU {

class TextureDecoder {
  public:
    TextureDecoder();
    ~TextureDecoder();

    void Decode(const Renderer::TextureDescriptor& descriptor,
                u8* scratch_buffer, u8*& out_data);

  private:
};

} // namespace Hydra::HW::TegraX1::GPU
