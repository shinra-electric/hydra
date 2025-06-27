#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/hid.hpp"

namespace hydra::horizon::applets::controller {

enum class ControllerSupportMode : u8 {
    ShowControllerSupport = 0,
    ShowControllerStrapGuide = 1,            // 3.0.0+
    ShowControllerFirmwareUpdate = 2,        // 3.0.0+
    ShowControllerKeyRemappingForSystem = 4, // 11.0.0+
};

enum class ControllerSupportCaller : u8 {
    Application = 0,
    SystemSettings = 1, // Skips the firmware-update confirmation dialog (TODO:
                        // SystemSettings?)
};

struct ControllerSupportArgPrivate {
    u32 size;
    u32 controller_support_arg_size;
    u8 flag0; // TODO: what is this?
    u8 flag1; // TODO: what is this?
    ControllerSupportMode mode;
    ControllerSupportCaller caller;
    hid::NpadStyleSet npad_style_set;
    hid::NpadJoyHoldType npad_joy_hold_type;
};

struct ControllerSupportArg0 {
    i8 player_count_min;
    i8 player_count_max;
    bool enable_take_over_connection; // Disconnects the controllers when not
                                      // enabled (TODO: what is this?)
    bool enable_left_justify;         // TODO: what is this?
    bool enable_permit_joy_dual;
    bool enable_single_mode;
    bool enable_identification_color;
    uchar4 identification_colors[];
};

struct ControllerSupportArg1 {
    bool enable_explain_text;
};

} // namespace hydra::horizon::applets::controller
