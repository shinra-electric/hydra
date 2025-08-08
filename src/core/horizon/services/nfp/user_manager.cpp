#include "core/horizon/services/nfp/user_manager.hpp"

#include "core/horizon/services/nfp/user.hpp"

namespace hydra::horizon::services::nfp {

DEFINE_SERVICE_COMMAND_TABLE(IUserManager, 0, CreateUserInterface)

result_t IUserManager::CreateUserInterface(RequestContext* ctx) {
    AddService(*ctx, new IUser());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::nfp
