#include "core/horizon/services/account/manager_for_application.hpp"

namespace hydra::horizon::services::account {

DEFINE_SERVICE_COMMAND_TABLE(IManagerForApplication, 0, CheckAvailability, 1,
                             GetAccountId)

result_t IManagerForApplication::CheckAvailability() {
    LOG_FUNC_NOT_IMPLEMENTED(Services);
    return RESULT_SUCCESS;
}

result_t IManagerForApplication::GetAccountId() {
    LOG_FUNC_NOT_IMPLEMENTED(Services);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
