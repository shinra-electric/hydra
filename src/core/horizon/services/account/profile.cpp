#include "core/horizon/services/account/profile.hpp"

// TODO: make all of this configurable

namespace hydra::horizon::services::account {

namespace {

struct AccountUserData {
    u32 unk_x0;
    u32 icon_id;
    u8 icon_background_color_id;
    u8 unk_x9[0x7];
    u8 mii_id[0x10];
    u8 unk_x20[0x60];
};

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IProfile, 0, Get, 1, GetBase)

result_t
IProfile::Get(AccountProfileBase* out_base,
              OutBuffer<BufferAttr::HipcPointer> out_user_data_buffer) {
    // TODO: get this from state manager
    AccountUserData user_data{
        .unk_x0 = 0,
        .icon_id = 0,
        .icon_background_color_id = 0,
        .unk_x9 = {0},
        .mii_id = {0},
        .unk_x20 = {0},
    };
    out_user_data_buffer.writer->Write(user_data);

    GetBase(out_base);

    return RESULT_SUCCESS;
}

result_t IProfile::GetBase(AccountProfileBase* out_base) {
    // TODO: get this from state manager
    *out_base = {
        .uid = user_id,
        .last_edit_timestamp = 0,
        .nickname = "hydra user",
    };

    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
