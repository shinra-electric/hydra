#include "core/horizon/services/account/account_service_for_application.hpp"

#include "core/horizon/services/account/manager_for_application.hpp"
#include "core/horizon/services/account/profile.hpp"
#include "core/horizon/services/account/user_manager.hpp"

namespace hydra::horizon::services::account {

DEFINE_SERVICE_COMMAND_TABLE(IAccountServiceForApplication, 1, GetUserExistence,
                             4, GetLastOpenedUser, 5, GetProfile, 100,
                             InitializeApplicationInfoV0, 101,
                             GetBaasAccountManagerForApplication, 140,
                             InitializeApplicationInfo, 150,
                             IsUserAccountSwitchLocked)

result_t IAccountServiceForApplication::GetUserExistence(uuid_t user_id,
                                                         bool* out_exists) {
    LOG_DEBUG(Services, "User ID: 0x{:08x}", user_id);

    *out_exists = USER_MANAGER_INSTANCE.Exists(user_id);
    return RESULT_SUCCESS;
}

result_t IAccountServiceForApplication::GetLastOpenedUser(uuid_t* out_user_id) {
    LOG_FUNC_STUBBED(Services);

    // HACK: return the first user
    *out_user_id = *USER_MANAGER_INSTANCE.Begin();
    return RESULT_SUCCESS;
}

result_t IAccountServiceForApplication::GetProfile(add_service_fn_t add_service,
                                                   uuid_t user_id) {
    add_service(new IProfile(user_id));
    return RESULT_SUCCESS;
}

result_t IAccountServiceForApplication::GetBaasAccountManagerForApplication(
    add_service_fn_t add_service, uuid_t user_id) {
    add_service(new IManagerForApplication(user_id));
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
