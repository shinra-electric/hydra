#include "core/horizon/services/account/account_service_for_application.hpp"

#include "core/horizon/services/account/manager_for_application.hpp"
#include "core/horizon/services/account/profile.hpp"

namespace Hydra::Horizon::Services::Account {

DEFINE_SERVICE_COMMAND_TABLE(IAccountServiceForApplication, 1, GetUserExistence,
                             5, GetProfile, 100, InitializeApplicationInfoV0,
                             101, GetBaasAccountManagerForApplication)

result_t IAccountServiceForApplication::GetUserExistence(bool* out_exists) {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);

    // HACK
    *out_exists = true;
    return RESULT_SUCCESS;
}

result_t IAccountServiceForApplication::GetProfile(add_service_fn_t add_service,
                                                   u128 user_id) {
    add_service(new IProfile(user_id));
    return RESULT_SUCCESS;
}

result_t IAccountServiceForApplication::GetBaasAccountManagerForApplication(
    add_service_fn_t add_service, u128 user_id) {
    add_service(new IManagerForApplication(user_id));
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Account
