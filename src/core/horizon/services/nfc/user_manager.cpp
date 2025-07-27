#include "core/horizon/services/nfc/user_manager.hpp"

#include "core/horizon/services/nfc/mifare/user.hpp"

namespace hydra::horizon::services::nfc {

DEFINE_SERVICE_COMMAND_TABLE(IUserManager, 0, CreateUserInterface)

result_t IUserManager::CreateUserInterface(RequestContext* ctx) {
    AddService(*ctx, new mifare::IUser());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::nfc
