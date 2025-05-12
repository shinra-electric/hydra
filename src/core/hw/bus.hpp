#pragma once

#include "common/common.hpp"

namespace hydra::hw {

namespace Display {
class Display;
}

class Bus {
  public:
    void ConnectDisplay(Display::Display* display, u32 display_id) {
        displays[display_id] = display;
    }

    // Getters
    Display::Display* GetDisplay(u32 display_id) const {
        return displays[display_id];
    }

  private:
    Display::Display*
        displays[2]; // TODO: what's the maximum number of displays?
};

} // namespace hydra::hw
