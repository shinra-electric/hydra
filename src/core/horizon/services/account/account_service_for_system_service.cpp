#include "core/horizon/services/account/account_service_for_system_service.hpp"

#include "core/horizon/services/account/baas/manager_for_system_service.hpp"
#include "core/horizon/services/account/notifier.hpp"

namespace hydra::horizon::services::account {

DEFINE_SERVICE_COMMAND_TABLE(IAccountServiceForSystemService, 0, GetUserCount,
                             1, GetUserExistence, 2, ListAllUsers, 3,
                             ListOpenUsers, 4, GetLastOpenedUser, 5, GetProfile,
                             100, GetUserRegistrationNotifier, 101,
                             GetUserStateChangeNotifier, 102,
                             GetBaasAccountManagerForSystemService)

result_t IAccountServiceForSystemService::GetUserRegistrationNotifier(
    RequestContext* ctx) {
    AddService(*ctx, new INotifier());
    return RESULT_SUCCESS;
}

result_t IAccountServiceForSystemService::GetUserStateChangeNotifier(
    RequestContext* ctx) {
    AddService(*ctx, new INotifier());
    return RESULT_SUCCESS;
}

result_t IAccountServiceForSystemService::GetBaasAccountManagerForSystemService(
    RequestContext* ctx) {
    AddService(*ctx, new baas::IManagerForSystemService());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
