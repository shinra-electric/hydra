#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

#include "common/logging/log.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/texture.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

static Renderer* s_instance;

Renderer& Renderer::GetInstance() { return *s_instance; }

Renderer::Renderer() {
    ASSERT(s_instance == nullptr, GPU, "Metal renderer already exists");
    s_instance = this;

    device = MTL::CreateSystemDefaultDevice();
    command_queue = device->newCommandQueue();
}

Renderer::~Renderer() {
    command_queue->release();
    device->release();

    s_instance = nullptr;
}

void Renderer::SetSurface(void* surface) {
    layer = reinterpret_cast<CA::MetalLayer*>(surface);
    layer->setDevice(device);
}

void Renderer::Present(TextureBase* texture) {
    LOG_WARNING(MetalRenderer, "Not implemented");
}

TextureBase* Renderer::CreateTexture(const TextureDescriptor& descriptor) {
    return new Texture(descriptor);
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
