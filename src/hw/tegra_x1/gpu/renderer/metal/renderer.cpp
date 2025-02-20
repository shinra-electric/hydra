#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

Renderer::Renderer() {
    device = MTL::CreateSystemDefaultDevice();
    command_queue = device->newCommandQueue();
}

Renderer::~Renderer() {
    command_queue->release();
    device->release();
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
