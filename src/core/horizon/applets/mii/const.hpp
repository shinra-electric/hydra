#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon::applets::mii {

enum class AppletMode : u32 {
    ShowMiiEdit = 0,
    AppendMii = 1,
    AppendMiiImage = 2,
    UpdateMiiImage = 3,
    CreateMii = 4, // 10.2.0+
    EditMii = 5,   // 10.2.0+
};

struct AppletInput {
    i32 _unknown_x0; // 0x3 (10.2.0+ 0x4)
    AppletMode mode;
    u32 special_mii_key_code; // TODO: what is this?
    uuid_t valid_uuid_array[8];
    u8 char_info[0x58]; // 10.2.0+ (TODO: what is this?)
    uuid_t used_uuid;
    u8 _unused_x9c[0x64];
};

} // namespace hydra::horizon::applets::mii
