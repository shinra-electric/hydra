#pragma once

#include "core/horizon/display/binder.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {
class TextureBase;
}

namespace hydra::horizon::display {

// TODO: should have its own framebuffer
class Layer {
  public:
    Layer(u32 binder_id_) : binder_id{binder_id_} {}

    // TODO
    void Open() {}
    void Close() {}

    bool AcquirePresentTexture();
    void Present(u32 width, u32 height);

    // Time
    AccumulatedTime GetAccumulatedDT();

  private:
    u32 binder_id;

    hw::tegra_x1::gpu::renderer::TextureBase* present_texture{nullptr};
    BqBufferInput input;

  public:
    GETTER(binder_id, GetBinderID);
};

} // namespace hydra::horizon::display
