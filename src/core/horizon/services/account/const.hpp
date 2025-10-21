#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::account {

constexpr usize NICKNAME_SIZE = 0x20;

struct ProfileBase {
    uuid_t user_id;
    u64 last_edit_timestamp;
    char nickname[0x20];
};

struct UserData {
    u32 unk_x0;
    u32 icon_id;
    u8 icon_background_color_id;
    u8 unk_x9[0x7];
    u8 mii_id[0x10];
    u8 unk_x20[0x60];
};

} // namespace hydra::horizon::services::account
