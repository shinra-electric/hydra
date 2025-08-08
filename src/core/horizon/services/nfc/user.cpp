#include "core/horizon/services/nfc/user.hpp"

namespace hydra::horizon::services::nfc {

DEFINE_SERVICE_COMMAND_TABLE(IUser, 0, Initialize, 2, GetState, 400, Initialize,
                             402, GetState)

} // namespace hydra::horizon::services::nfc
