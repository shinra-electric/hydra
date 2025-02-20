#pragma once

#include "common/common.hpp"

namespace Hydra::HW::Display {

class LayerBase {
  public:
    LayerBase(u32 binder_id_) : binder_id{binder_id_} {}

    // TODO: are these needed?
    virtual void Open() = 0;
    virtual void Close() = 0;

    // Getters
    u32 GetBinderId() { return binder_id; }

  private:
    u32 binder_id;
};

} // namespace Hydra::HW::Display
