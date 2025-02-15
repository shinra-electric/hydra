#include "horizon/services/visrv/manager_root_service.hpp"

#include "horizon/services/visrv/application_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

DEFINE_SERVICE_COMMAND_TABLE(IManagerRootService, 0, GetDisplayService, 2,
                             GetDisplayServiceWithProxyNameExchange)

void IManagerRootService::GetDisplayService(REQUEST_COMMAND_PARAMS) {
    add_service(new IApplicationDisplayService());
}

void IManagerRootService::GetDisplayServiceWithProxyNameExchange(
    REQUEST_COMMAND_PARAMS) {
    // TODO: should take input u64 and u32
    add_service(new IApplicationDisplayService());
}

} // namespace Hydra::Horizon::Services::ViSrv
