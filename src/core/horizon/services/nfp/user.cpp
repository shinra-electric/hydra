#include "core/horizon/services/nfp/user.hpp"

namespace hydra::horizon::services::nfp {

DEFINE_SERVICE_COMMAND_TABLE(IUser, 0, Initialize, 2, ListDevices, 19, GetState)

} // namespace hydra::horizon::services::nfp
