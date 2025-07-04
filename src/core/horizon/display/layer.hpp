#pragma once

namespace hydra::horizon::display {

// TODO: should have its own framebuffer
class Layer {
  public:
    Layer(u32 binder_id_) : binder_id{binder_id_} {}

    // TODO
    void Open() {}
    void Close() {}

  private:
    u32 binder_id;

  public:
    GETTER(binder_id, GetBinderID);
};

} // namespace hydra::horizon::display
