#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon::applets {

struct CommonArguments {
    u32 version; // Version of the struct
    u32 size;    // Size of the struct
    u32 library_applet_api_version;
    u32 theme_color;
    bool play_startup_sound;
    u64 system_tick;
};

} // namespace hydra::horizon::applets
