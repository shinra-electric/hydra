#pragma once

#include "hw/display/layer_base.hpp"
#include "hw/display/sdl3/const.hpp"

namespace Hydra::HW::Display::SDL3 {

class Layer : public LayerBase {
  public:
    Layer(u32 binder_id);

    void Open() override {}
    void Close() override {}

  private:
};

} // namespace Hydra::HW::Display::SDL3
