#include "core/horizon/services/am/application_proxy_service.hpp"

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/services/am/application_proxy.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationProxyService, 0, OpenApplicationProxy)

result_t
IApplicationProxyService::OpenApplicationProxy(add_service_fn_t add_service) {
    add_service(new IApplicationProxy());
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Am
