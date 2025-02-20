#pragma once

#include "common/common.hpp"

namespace Hydra::HW {

namespace Display {
class DisplayBase;
}

class Bus {
  public:
    void ConnectDisplay(Display::DisplayBase* display, u32 display_id) {
        displays[display_id] = display;
    }

    // Getters
    Display::DisplayBase* GetDisplay(u32 display_id) const {
        return displays[display_id];
    }

  private:
    Display::DisplayBase*
        displays[2]; // TODO: what's the maximum number of displays?
};

} // namespace Hydra::HW
