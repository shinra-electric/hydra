#include "core/input/apple_gc/keyboard.hpp"

namespace hydra::input::apple_gc {

bool Keyboard::IsPressedImpl(Key key) {
    ONCE(LOG_FUNC_NOT_IMPLEMENTED(Input));
    return false;
}

} // namespace hydra::input::apple_gc
