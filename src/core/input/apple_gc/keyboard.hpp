#pragma once

#include "core/input/keyboard_base.hpp"

namespace hydra::input::apple_gc {

class Keyboard : public KeyboardBase {
  protected:
    bool IsPressedImpl(Key key) override;
};

} // namespace hydra::input::apple_gc
