#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::account {

enum class AccountServiceType {
    Administrator,
    SystemService,
    Application,
    BaasAccessTokenAccessor,
};

class IAccountService : public IService {
  public:
    IAccountService(AccountServiceType type_) : type{type_} {}

  protected:
    // Commands
    result_t GetUserCount(i32* out_count);
    result_t GetUserExistence(uuid_t user_id, bool* out_exists);
    result_t ListAllUsers(OutBuffer<BufferAttr::HipcPointer> out_buffer);
    // TODO: correct?
    result_t ListOpenUsers(OutBuffer<BufferAttr::HipcPointer> out_buffer);
    result_t GetLastOpenedUser(uuid_t* out_user_id);
    result_t GetProfile(RequestContext* ctx, uuid_t user_id);
    // TODO: PID and PID reserved
    result_t IsUserRegistrationRequestPermitted(bool* out_permitted);

  private:
    AccountServiceType type;
};

} // namespace hydra::horizon::services::account
