#include "hw/tegra_x1/gpu/renderer/metal/texture.hpp"

#include "hw/tegra_x1/gpu/renderer/metal/maxwell_to_mtl.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

Texture::Texture(const TextureDescriptor& descriptor)
    : TextureBase(descriptor) {
    MTL::TextureDescriptor* desc = MTL::TextureDescriptor::alloc()->init();
    desc->setWidth(descriptor.width);
    desc->setHeight(descriptor.height);

    pixel_format = get_mtl_pixel_format(descriptor.surface_format);
    desc->setPixelFormat(pixel_format);

    texture = Renderer::GetInstance().GetDevice()->newTexture(desc);
}

Texture::~Texture() { texture->release(); }

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
