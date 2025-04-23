#include "core/horizon/services/account/manager_for_application.hpp"

namespace Hydra::Horizon::Services::Account {

DEFINE_SERVICE_COMMAND_TABLE(IManagerForApplication, 0, CheckAvailability, 1, GetAccountId)

void IManagerForApplication::CheckAvailability(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
}

void IManagerForApplication::GetAccountId(REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
}

} // namespace Hydra::Horizon::Services::Account
