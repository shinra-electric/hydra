#include "core/horizon/services/account/profile.hpp"

#include "core/horizon/os.hpp"

namespace hydra::horizon::services::account {

DEFINE_SERVICE_COMMAND_TABLE(IProfile, 0, Get, 1, GetBase, 10, GetImageSize, 11,
                             LoadImage)

result_t
IProfile::Get(ProfileBase* out_base,
              OutBuffer<BufferAttr::HipcPointer> out_user_data_buffer) {
    const auto& user = USER_MANAGER_INSTANCE.Get(user_id);
    *out_base = user.GetBase();
    out_user_data_buffer.writer->Write(user.GetData());
    return RESULT_SUCCESS;
}

result_t IProfile::GetBase(ProfileBase* out_base) {
    const auto& user = USER_MANAGER_INSTANCE.Get(user_id);
    *out_base = user.GetBase();
    return RESULT_SUCCESS;
}

result_t IProfile::GetImageSize(u32* out_size) {
    *out_size = USER_MANAGER_INSTANCE.GetAvatarImageSize(user_id);
    return RESULT_SUCCESS;
}

result_t IProfile::LoadImage(OutBuffer<BufferAttr::MapAlias> out_buffer,
                             u32* out_size) {
    // Load images
    u8* bg_data;
    USER_MANAGER_INSTANCE.LoadAvatarBackgroundImage(user_id, bg_data);
    u8* char_data;
    USER_MANAGER_INSTANCE.LoadAvatarCharacterImage(user_id, char_data);

    const auto size = USER_MANAGER_INSTANCE.GetAvatarImageSize(user_id);

    // Alpha blend the background and character images
    for (u32 i = 0; i < size / 4; i++) {
        u8 r1 = bg_data[i * 4 + 0];
        u8 g1 = bg_data[i * 4 + 1];
        u8 b1 = bg_data[i * 4 + 2];
        u8 a1 = bg_data[i * 4 + 3];

        u8 r2 = char_data[i * 4 + 0];
        u8 g2 = char_data[i * 4 + 1];
        u8 b2 = char_data[i * 4 + 2];
        u8 a2 = char_data[i * 4 + 3];

        u8 r = (r1 * (255 - a2) + r2 * a2) / 255;
        u8 g = (g1 * (255 - a2) + g2 * a2) / 255;
        u8 b = (b1 * (255 - a2) + b2 * a2) / 255;

        out_buffer.writer->Write(r);
        out_buffer.writer->Write(g);
        out_buffer.writer->Write(b);
        out_buffer.writer->Write(a1);
    }

    // Cleanup
    free(bg_data);
    free(char_data);

    *out_size = size;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
