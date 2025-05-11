#include "core/horizon/services/socket/resolver/resolver.hpp"

namespace Hydra::Horizon::Services::Socket::Resolver {

DEFINE_SERVICE_COMMAND_TABLE(IResolver, 6, GetAddrInfoRequest)

result_t IResolver::GetAddrInfoRequest() {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Socket::Resolver
