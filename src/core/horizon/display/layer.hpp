#pragma once

#include "core/horizon/display/binder.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {
class TextureBase;
}

namespace hydra::horizon::display {

// TODO: should have its own framebuffer
class Layer {
  public:
    Layer(kernel::Process* process_, u32 binder_id_)
        : process{process_}, binder_id{binder_id_} {}

    // TODO
    void Open() {}
    void Close() {}

    bool AcquirePresentTexture();
    void Present(float2 dst_origin, f32 dst_scale, bool transparent);

    // Time
    AccumulatedTime GetAccumulatedDT();

  private:
    kernel::Process* process;
    u32 binder_id;

    float2 position{0, 0};
    uint2 size{0, 0}; // (0, 0) means auto
    i64 z{0};

    // Present
    hw::tegra_x1::gpu::renderer::TextureBase* present_texture{nullptr};
    IntRect2D src_rect;

  public:
    GETTER(process, GetProcess);
    GETTER(binder_id, GetBinderID);
    SETTER(position, SetPosition);
    SETTER(size, SetSize);
    GETTER_AND_SETTER(z, GetZ, SetZ);
    GETTER(present_texture, GetPresentTexture);
    CONST_REF_GETTER(src_rect, GetSrcRect);
};

} // namespace hydra::horizon::display
