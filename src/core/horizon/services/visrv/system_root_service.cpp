#include "core/horizon/services/visrv/system_root_service.hpp"

#include "core/horizon/services/visrv/application_display_service.hpp"

namespace hydra::horizon::services::visrv {

DEFINE_SERVICE_COMMAND_TABLE(ISystemRootService, 1, GetDisplayService, 3,
                             GetDisplayServiceWithProxyNameExchange)

result_t ISystemRootService::GetDisplayService(RequestContext* ctx) {
    // TODO: should take input u32
    AddService(*ctx, new IApplicationDisplayService());
    return RESULT_SUCCESS;
}

result_t ISystemRootService::GetDisplayServiceWithProxyNameExchange(
    RequestContext* ctx) {
    AddService(*ctx, new IApplicationDisplayService());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::visrv
