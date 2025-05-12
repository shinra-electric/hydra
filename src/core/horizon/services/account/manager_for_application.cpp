#include "core/horizon/services/account/manager_for_application.hpp"

namespace hydra::horizon::services::account {

DEFINE_SERVICE_COMMAND_TABLE(IManagerForApplication, 0, CheckAvailability, 1,
                             GetAccountId)

result_t IManagerForApplication::GetAccountId(uuid_t* out_id) {
    *out_id = user_id;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::account
