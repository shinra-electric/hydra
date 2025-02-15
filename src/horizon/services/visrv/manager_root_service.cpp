#include "horizon/services/visrv/manager_root_service.hpp"

#include "horizon/services/visrv/application_display_service.hpp"

namespace Hydra::Horizon::Services::ViSrv {

void IManagerRootService::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 0: // GetDisplayService
        add_service(new IApplicationDisplayService());
        break;
    case 2: // GetDisplayServiceWithProxyNameExchange
        // TODO: should take input u64 and u32
        add_service(new IApplicationDisplayService());
        break;
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::ViSrv
