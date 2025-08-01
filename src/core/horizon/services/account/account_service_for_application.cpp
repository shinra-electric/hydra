#include "core/horizon/services/account/account_service_for_application.hpp"

#include "core/horizon/services/account/manager_for_application.hpp"

namespace hydra::horizon::services::account {

DEFINE_SERVICE_COMMAND_TABLE(IAccountServiceForApplication, 0, GetUserCount, 1,
                             GetUserExistence, 2, ListAllUsers, 3,
                             ListOpenUsers, 4, GetLastOpenedUser, 5, GetProfile,
                             100, InitializeApplicationInfoV0, 101,
                             GetBaasAccountManagerForApplication, 140,
                             InitializeApplicationInfo, 150,
                             IsUserAccountSwitchLocked, 160,
                             InitializeApplicationInfoV2)

result_t IAccountServiceForApplication::GetBaasAccountManagerForApplication(
    RequestContext* ctx, uuid_t user_id) {
    AddService(*ctx, new IManagerForApplication(user_id));
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
