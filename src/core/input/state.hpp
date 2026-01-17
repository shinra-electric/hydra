#pragma once

#include "core/input/const.hpp"

namespace hydra::input {

struct NpadState {
    horizon::services::hid::NpadButtons buttons{
        horizon::services::hid::NpadButtons::None};
    f32 analog_l_x{0.0f};
    f32 analog_l_y{0.0f};
    f32 analog_r_x{0.0f};
    f32 analog_r_y{0.0f};
};

struct TouchState {
    u32 x;
    u32 y;
    // TODO: more
};

} // namespace hydra::input
