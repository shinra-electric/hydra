#pragma once

#include "common/common.hpp"

namespace Hydra::HW::Display {

class Layer {
  public:
    Layer(u32 binder_id_) : binder_id{binder_id_} {}

    // TODO: are these needed?
    void Open() {}
    void Close() {}

    // Getters
    u32 GetBinderId() { return binder_id; }

  private:
    u32 binder_id;
};

} // namespace Hydra::HW::Display
