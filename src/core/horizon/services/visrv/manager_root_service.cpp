#include "core/horizon/services/visrv/manager_root_service.hpp"

#include "core/horizon/services/visrv/application_display_service.hpp"

namespace hydra::horizon::services::visrv {

// TODO: is this really correct?
DEFINE_SERVICE_COMMAND_TABLE(IManagerRootService, 0, GetDisplayService, 2,
                             GetDisplayServiceWithProxyNameExchange)

result_t IManagerRootService::GetDisplayService(RequestContext* ctx) {
    AddService(*ctx, new IApplicationDisplayService());
    return RESULT_SUCCESS;
}

result_t IManagerRootService::GetDisplayServiceWithProxyNameExchange(
    RequestContext* ctx) {
    // TODO: should take input u64 and u32
    AddService(*ctx, new IApplicationDisplayService());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::visrv
