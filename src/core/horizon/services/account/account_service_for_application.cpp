#include "core/horizon/services/account/account_service_for_application.hpp"

#include "core/horizon/services/account/manager_for_application.hpp"
#include "core/horizon/services/account/profile.hpp"

namespace Hydra::Horizon::Services::Account {

DEFINE_SERVICE_COMMAND_TABLE(IAccountServiceForApplication, 5, GetProfile, 100,
                             InitializeApplicationInfoV0, 101,
                             GetBaasAccountManagerForApplication)

void IAccountServiceForApplication::GetProfile(REQUEST_COMMAND_PARAMS) {
    const u128 user_id = readers.reader.Read<u128>();
    add_service(new IProfile(user_id));
}

void IAccountServiceForApplication::GetBaasAccountManagerForApplication(
    REQUEST_COMMAND_PARAMS) {
    const auto user_id = readers.reader.Read<u128>();
    add_service(new IManagerForApplication(user_id));
}

} // namespace Hydra::Horizon::Services::Account
