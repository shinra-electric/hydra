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

// TODO: load without an intermediate buffer?
result_t IProfile::LoadImage(OutBuffer<BufferAttr::MapAlias> out_buffer,
                             u32* out_size) {
    // Load image
    std::vector<u8> data;
    USER_MANAGER_INSTANCE.LoadAvatarImage(user_id, data);

    out_buffer.writer->WritePtr(data.data(), data.size());

    *out_size = data.size();
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
