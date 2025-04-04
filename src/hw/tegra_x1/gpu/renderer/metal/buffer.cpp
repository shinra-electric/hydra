#include "hw/tegra_x1/gpu/renderer/metal/buffer.hpp"

#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

Buffer::Buffer(const BufferDescriptor& descriptor) : BufferBase(descriptor) {
    buffer = Renderer::GetInstance().GetDevice()->newBuffer(
        reinterpret_cast<void*>(descriptor.ptr), descriptor.size,
        MTL::ResourceStorageModeShared, nullptr);
}

Buffer::Buffer(MTL::Buffer* buffer_, u32 offset_)
    : BufferBase({reinterpret_cast<uptr>(buffer_->contents()) + offset_,
                  buffer_->allocatedSize()}),
      buffer{buffer_}, offset{offset_} {
    owns_buffer = false;
}

Buffer::~Buffer() {
    if (owns_buffer)
        buffer->release();
}

void Buffer::CopyFrom(const void* data) {
    // TODO: get the buffer from a buffer allocator instead
    auto tmp_buffer = Renderer::GetInstance().GetDevice()->newBuffer(
        descriptor.size, MTL::ResourceStorageModeShared);
    memcpy(tmp_buffer->contents(), data, descriptor.size);

    auto blit_encoder = Renderer::GetInstance().GetBlitCommandEncoder();
    blit_encoder->copyFromBuffer(tmp_buffer, 0, buffer, 0, descriptor.size);

    tmp_buffer->release();
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
