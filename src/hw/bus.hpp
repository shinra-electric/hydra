#pragma once

#include "common/common.hpp"

namespace Hydra::HW {

namespace Display {
class DisplayBase;
}

class Bus {
  public:
    // Getters
    Display::DisplayBase* GetDisplay(u32 display_id) const {
        return displays[display_id];
    }

    // Setters
    void SetDisplay(Display::DisplayBase* display, u32 display_id) {
        displays[display_id] = display;
    }

  private:
    Display::DisplayBase*
        displays[2]; // TODO: what's the maximum number of displays?
};

} // namespace Hydra::HW
