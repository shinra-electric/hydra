#include "hw/tegra_x1/gpu/renderer/metal/buffer.hpp"

#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

Buffer::Buffer(const BufferDescriptor& descriptor) : BufferBase(descriptor) {
    buffer = Renderer::GetInstance().GetDevice()->newBuffer(
        reinterpret_cast<void*>(descriptor.ptr), descriptor.size,
        MTL::ResourceStorageModeShared, nullptr);
}

Buffer::~Buffer() { buffer->release(); }

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
