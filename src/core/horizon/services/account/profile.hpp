#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Account {

struct AccountProfileBase {
    u128 uid;
    u64 last_edit_timestamp;
    char nickname[0x20];
};

class IProfile : public ServiceBase {
  public:
    IProfile(u128 user_id_) : user_id{user_id_} {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    u128 user_id;

    // Commands
    result_t Get(AccountProfileBase* out_base,
                 OutBuffer<BufferAttr::HipcPointer> out_user_data_buffer);
    result_t GetBase(AccountProfileBase* out_base);
};

} // namespace Hydra::Horizon::Services::Account
