#include "core/horizon/services/account/account_service_for_system_service.hpp"

namespace hydra::horizon::services::account {

DEFINE_SERVICE_COMMAND_TABLE(IAccountServiceForSystemService, 0, GetUserCount,
                             1, GetUserExistence, 2, ListAllUsers, 3,
                             ListOpenUsers, 4, GetLastOpenedUser, 5, GetProfile)

} // namespace hydra::horizon::services::account
