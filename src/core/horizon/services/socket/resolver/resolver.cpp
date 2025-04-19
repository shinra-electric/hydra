#include "core/horizon/services/socket/resolver/resolver.hpp"

namespace Hydra::Horizon::Services::Socket::Resolver {

DEFINE_SERVICE_COMMAND_TABLE(IResolver, 6, GetAddrInfoRequest)

void IResolver::GetAddrInfoRequest(REQUEST_COMMAND_PARAMS) {
    LOG_NOT_IMPLEMENTED(HorizonServices, "GetAddrInfoRequest");
}

} // namespace Hydra::Horizon::Services::Socket::Resolver
