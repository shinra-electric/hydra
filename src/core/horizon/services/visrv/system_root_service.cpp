#include "core/horizon/services/visrv/system_root_service.hpp"

#include "core/horizon/services/visrv/application_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

DEFINE_SERVICE_COMMAND_TABLE(ISystemRootService, 1, GetDisplayService, 3,
                             GetDisplayServiceWithProxyNameExchange)

void ISystemRootService::GetDisplayService(REQUEST_COMMAND_PARAMS) {
    // TODO: should take input u32
    add_service(new IApplicationDisplayService());
}

void ISystemRootService::GetDisplayServiceWithProxyNameExchange(
    REQUEST_COMMAND_PARAMS) {
    add_service(new IApplicationDisplayService());
}

} // namespace Hydra::Horizon::Services::ViSrv
