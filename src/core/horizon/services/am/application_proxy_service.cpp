#include "core/horizon/services/am/application_proxy_service.hpp"

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/am/application_proxy.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationProxyService, 0, OpenApplicationProxy)

void IApplicationProxyService::OpenApplicationProxy(REQUEST_COMMAND_PARAMS) {
    add_service(new IApplicationProxy());
}

} // namespace Hydra::Horizon::Services::Am
