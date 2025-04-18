#include "core/horizon/services/visrv/application_root_service.hpp"

#include "core/horizon/services/visrv/application_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

DEFINE_SERVICE_COMMAND_TABLE(IApplicationRootService, 0, GetDisplayService, 1,
                             GetDisplayServiceWithProxyNameExchange)

void IApplicationRootService::GetDisplayService(REQUEST_COMMAND_PARAMS) {
    // TODO: should take input u32
    add_service(new IApplicationDisplayService());
}

void IApplicationRootService::GetDisplayServiceWithProxyNameExchange(
    REQUEST_COMMAND_PARAMS) {
    // TODO: should take input u32
    add_service(new IApplicationDisplayService());
}

} // namespace Hydra::Horizon::Services::ViSrv
