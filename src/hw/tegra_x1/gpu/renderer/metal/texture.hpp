#pragma once

#include "hw/tegra_x1/gpu/renderer/metal/const.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

class Texture final : public TextureBase {
  public:
    Texture(const TextureDescriptor& descriptor);
    ~Texture() override;

    // Copying
    void CopyFrom(const void* data) override;
    void CopyFrom(const BufferBase* src, const usize src_stride,
                  const u32 dst_layer, const uint3 dst_origin,
                  const usize3 size) override;

    // Getters
    MTL::Texture* GetTexture() const { return mtl_texture; }

    MTL::PixelFormat GetPixelFormat() const { return pixel_format; }

  private:
    MTL::Texture* mtl_texture;

    MTL::PixelFormat pixel_format;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
